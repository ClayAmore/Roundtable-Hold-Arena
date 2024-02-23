#include "P2PLobby.h"

namespace P2P {

	P2PLobby::P2PLobby(ISteamUser* steamUser, ISteamMatchmaking* steamMatchMaking)
	{
		_steamUser = steamUser;
		_steamMatchMaking = steamMatchMaking;
		_brawlManager = new BrawlManager();
		_teamFightManager = new TeamFightManager();
		_netChrsetSyncId = 0;
	}

	// Steam User
	bool P2PLobby::IsHost() {
		CSteamID selfSteamid = _steamUser->GetSteamID();
		CSteamID lobbyOwner = _steamMatchMaking->GetLobbyOwner(_lobbyId);
		return selfSteamid == lobbyOwner;
	}

	uint64_t P2PLobby::GetSelfSteamId() {
		return _steamUser->GetSteamID().ConvertToUint64();
	}

	// Callback for disconnect
	void P2PLobby::Cancel() {
		// Cancel if player is in active match
		if (LocalLobbyData.mode == Mode::Brawl) {
			_brawlManager->CancelMatch();
		}
		else if (LocalLobbyData.mode == Mode::TeamFight) {
			_teamFightManager->CancelMatch();
		}
	}

	void P2PLobby::Reset() {
		resetLocalData();
		UpdateLobbyMemberData();
	}

	// Lobby control
	void P2PLobby::OpenLobby() {
		if (!IsHost()) return;
		_steamMatchMaking->SetLobbyJoinable(_lobbyId, true);
	}

	void P2PLobby::CloseLobby() {
		if (!IsHost()) return;
		_steamMatchMaking->SetLobbyJoinable(_lobbyId, false);
	}

	void P2PLobby::CreateALobby() {
		_steamMatchMaking->CreateLobby(k_ELobbyTypePrivate, 20);
	}

	void P2PLobby::JoinLobby(uint64_t lobbyId) {
		_steamMatchMaking->JoinLobby(lobbyId);
	}

	void P2PLobby::RequestLobbyList() {
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
		m_CallResultLobbyMatchList.Set(hSteamAPICall, this, &P2PLobby::onLobbyMatchList);
	}

	bool P2PLobby::RequestLobbyData(CSteamID lobbyId) {
		return _steamMatchMaking->RequestLobbyData(lobbyId);
	}

	// Getters for local varibales
	uint32_t P2PLobby::GetMyChrSetSyncId() {
		return _netChrsetSyncId;
	}

	// Lobby Data
	bool P2PLobby::SyncLobbyData() {
		printf("Synchronizing data...\n");
		// Retrieve lobby data from Steam Matchmaking
		const char* serializedLobbyData = _steamMatchMaking->GetLobbyData(_lobbyId, LOBBY_DATA_KEY);

		printf("Serialized lobby data is: %s\n", serializedLobbyData);

		if (serializedLobbyData == nullptr || serializedLobbyData[0] == '\0') {
			printf("Serialized lobby data is missing or empty\n");
			return false;
		}    

		// Deserialize the lobby data and validate it
		P2PLobbyData lobbyData = P2PLobbyData::deserialize(serializedLobbyData);

		// Validate data
		if (!isValidP2PLobbyData(lobbyData)) {
			printf("Lobby data is not valid\n");
			return false;
		}

		// Update LocalLobbyData with the new data
		std::memcpy(&LocalLobbyData, &lobbyData, sizeof(P2PLobbyData));

		return true;
	}

	void P2PLobby::UpdateLobbyData() {
		// Update lobby data on Steam Matchmaking
		_steamMatchMaking->SetLobbyData(_lobbyId, LOBBY_DATA_KEY, LocalLobbyData.serialize().c_str());
	}


	// Chat
	void P2PLobby::onLobbyChatMsg(LobbyChatMsg_t* pCallback) {
		// Prepare variables for chatEntry retrieval.
		CSteamID sender;
		char messageData[MAX_MESSAGE_SIZE];
		int messageMaxSize = sizeof(messageData);
		EChatEntryType entryType;

		// Get messageData
		int size = _steamMatchMaking->GetLobbyChatEntry(pCallback->m_ulSteamIDLobby, pCallback->m_iChatID, &sender, messageData, messageMaxSize, &entryType);

		// Add message to the queue
		std::vector<char> messageBuffer(size);
		memcpy(messageBuffer.data(), messageData, size);
		messageQueue.push(messageBuffer);
	}
	
	void P2PLobby::HandleMessageQueue() {
		// Return if empty
		if (messageQueue.empty()) return;

		// Get a message from the queue
		auto message = messageQueue.front();

		// Check message header
		P2PLobbyMsgHeader* header = (P2PLobbyMsgHeader*)message.data();

		// Handle message
		switch (header->type) {
		case UpdateKills: {
			auto messageData = (P2PLobbyUpdateKillsMsg*)message.data();
			std::cout << "KilledId is " << messageData->killerId  << std::endl;
			std::cout << "My NetChrSetSyncId is " << _netChrsetSyncId << std::endl;
			if (messageData->killerId == _netChrsetSyncId) {
				MyLobbyMemberData.kills++;
				if (LocalLobbyData.mode == Mode::TeamFight) _teamFightManager->OnKillUpdate();
				else if (LocalLobbyData.mode == Mode::Brawl) _brawlManager->OnKillUpdate();
				UpdateLobbyMemberData();
			}
			else if (LocalLobbyData.mode == Mode::TeamFight) _teamFightManager->OnKillUpdate(MyLobbyMemberData.team, messageData->team);

			break;
		}
		}

		// Remove handled message from queue
		messageQueue.pop();
	}

	void P2PLobby::BroadcastMessageToLobbyMembers(const char* message, int size) {
		_steamMatchMaking->SendLobbyChatMsg(_lobbyId, message, size);
	}


	// Lobby Member Functions
	void P2PLobby::UpdateLobbyMemberData() {
		// Update lobby member data on Steam Matchmaking
		_steamMatchMaking->SetLobbyMemberData(_lobbyId, LOBBY_DATA_KEY, MyLobbyMemberData.serialize().c_str());
	}

	void P2PLobby::readLobbyMemberData(CSteamID lobbyMemberID, std::function<void(const P2PLobbyMemberData&)> callback) {

		// Retrieve lobby member data from Steam Matchmaking
		const char* serializedLobbyMemberData = _steamMatchMaking->GetLobbyMemberData(_lobbyId, lobbyMemberID, LOBBY_DATA_KEY);

		if (serializedLobbyMemberData == nullptr || serializedLobbyMemberData[0] == '\0') {
			printf("Serialized lobby member data is missing or empty\n");
			return;
		}

		// Deserialize the lobby member data
		P2PLobbyMemberData lobbyMemberData = P2PLobbyMemberData::deserialize(serializedLobbyMemberData);

		// Validate data
		if (!isValidP2PLobbyMemberData(lobbyMemberData)) {
			printf("Member data is invalid! Skipping member\n");
			return;
		}

		callback(lobbyMemberData);
	}

	void P2PLobby::iterateLobbyMembers(std::function<void(const P2PLobbyMemberData&)> callback) {
		int membersCount = _steamMatchMaking->GetNumLobbyMembers(_lobbyId);

		for (int i = 0; i < membersCount; i++) {
			CSteamID lobbyMemberID = _steamMatchMaking->GetLobbyMemberByIndex(_lobbyId, i);

			// Retrieve lobby member data from Steam Matchmaking
			const char* serializedLobbyMemberData = _steamMatchMaking->GetLobbyMemberData(_lobbyId, lobbyMemberID, LOBBY_DATA_KEY);

			if (serializedLobbyMemberData == nullptr || serializedLobbyMemberData[0] == '\0') {
				printf("Serialized lobby member data is missing or empty\n");
				return;
			}

			// Deserialize the lobby member data
			P2PLobbyMemberData lobbyMemberData = P2PLobbyMemberData::deserialize(serializedLobbyMemberData);

			// Validate data
			if (!isValidP2PLobbyMemberData(lobbyMemberData)) {
				printf("Member data is invalid! Skipping member\n");
				return;
			}

			callback(lobbyMemberData);
		}
	}


	// Helper functions
	bool P2PLobby::CheckIfEveryoneHasState(State state) {
		bool everyoneHasState = true;
		iterateLobbyMembers([&state, &everyoneHasState](const P2PLobbyMemberData& lobbyMemberData) {
			if (lobbyMemberData.state != state) everyoneHasState = false;
		});
		return everyoneHasState;
	}

	bool P2PLobby::isValidP2PLobbyData(const  P2PLobbyData& lobbyData) {
		// Perform validity checks on lobbyData fields
		// Return false if any checks fail, indicating invalid data

		if (lobbyData.mode > Mode::Brawl) {
			printf("LobbyData validation error: { Mode %d is not valid }\n", lobbyData.mode);
			return false;  // Invalid mode value
		}

		if (lobbyData.state > State::BrawlEndMatch) {
			printf("LobbyData validation error: { State %d is not valid }\n", lobbyData.state);
			return false;  // Invalid state value
		}

		if (lobbyData.blueTeamScore < 0 || lobbyData.redTeamScore < 0) {
			printf("LobbyData validation error: { blueTeamScore %d, redTeamScore :%d is not valid }\n", lobbyData.blueTeamScore, lobbyData.redTeamScore);
			return false;  // Scores should not be negative
		}

		if (lobbyData.mostKills < 0) {
			printf("LobbyData validation error: { mostKilss %d is not valid }\n", lobbyData.mostKills);
			return false;  // Most kills should not be negative
		}


		return true;  // All checks passed, indicating valid data
	}

	bool P2PLobby::isValidP2PLobbyMemberData(const P2PLobbyMemberData& memberData) {

		if (static_cast<State>(memberData.state) > State::BrawlEndMatch) {
			printf("MemberData validation error: { State %d is not valid }\n", memberData.state);
			return false;
		}

		if (static_cast<Seamless::Team>(memberData.team) > Seamless::RED) {
			printf("MemberData validation error: { team %d is not valid }\n", static_cast<Seamless::Team>(memberData.team));
			return false;
		}

		return true;
	}

	void P2PLobby::resetLocalData() {
		// Clear local lobby data
		LocalLobbyData.mode = Mode::None;
		LocalLobbyData.state = State::Nothing;
		LocalLobbyData.blueTeamScore = 0;
		LocalLobbyData.redTeamScore = 0;
		LocalLobbyData.mostKills = 0;
		LocalLobbyData.matchDuration = 0;
		LocalLobbyData.remainAfterMatch = Settings::modPreferences.remainAfterMatch;
		for (int i = 0; i < 32; i++) {
			LocalLobbyData.greatestCombatant[i] = '\0';
		}

		// Clear local memberdata
		MyLobbyMemberData.state = State::Nothing;
		MyLobbyMemberData.team = Seamless::GetSeamlessInfo().team;
		MyLobbyMemberData.kills = 0;
		MyLobbyMemberData.deaths = 0;
		_netChrsetSyncId = 0;
	}

	// Lobby data helpers
	void P2PLobby::recalculateAndUpdateTeamFightScore() {
		
		int16_t red_kills = 0;
		int16_t blue_kills = 0;
		iterateLobbyMembers([&red_kills, &blue_kills](const P2PLobbyMemberData& lobbyMemberData) {
			int16_t lobbyMemberDeaths = lobbyMemberData.deaths;
			if (lobbyMemberData.team == Seamless::BLUE) red_kills += lobbyMemberDeaths;
			else if (lobbyMemberData.team == Seamless::RED) blue_kills += lobbyMemberDeaths;
		});

		LocalLobbyData.blueTeamScore = blue_kills;
		LocalLobbyData.redTeamScore = red_kills;
		UpdateLobbyData();
	}

	void P2PLobby::recalculateAndUpdateMostKills(uint64_t memberSteamId) {
		
		P2PLobbyData localLobbyData = LocalLobbyData;
		uint16_t mostKills = 0;
		char greatestCombatant[32];
		bool updated = false;
		readLobbyMemberData(memberSteamId, [&localLobbyData, &mostKills, &greatestCombatant, &updated](const P2PLobbyMemberData& lobbyMemberData) {
			int16_t lobbyMemberKills = lobbyMemberData.kills;
			int16_t lobbyMostKills = localLobbyData.mostKills;
			if (lobbyMemberKills > lobbyMostKills) {
				mostKills = lobbyMemberData.kills;
				for (int i = 0; i < 32; i++) {
					greatestCombatant[i] = lobbyMemberData.charName[i];
					updated = true;
				}
			}
		});

		if (mostKills == 0) return;

		LocalLobbyData.mostKills = mostKills;
		if (updated) {
			for (int i = 0; i < 32; i++) {
				LocalLobbyData.greatestCombatant[i] = greatestCombatant[i];
				updated = true;
			}
		}
		UpdateLobbyData();
	}


	// Lobby Callbacks
	void P2PLobby::onLobbyEntered(LobbyEnter_t* lobbyEntered) {
		resetLocalData();
		_lobbyId = CSteamID(lobbyEntered->m_ulSteamIDLobby);

		std::string playerNameStr = Player::GetPlayerNameAsString();

		// Copy the player name to the charName array in MyLobbyMemberData
		if (playerNameStr.size() < sizeof(MyLobbyMemberData.charName)) {
			// The player name fits within the charName array, so we can safely use std::strcpy
			std::strcpy(MyLobbyMemberData.charName, playerNameStr.c_str());
		}
		else {
			// The player name is larger than the charName array, so we use std::strncpy to copy a limited number of characters
			std::strncpy(MyLobbyMemberData.charName, playerNameStr.c_str(), sizeof(MyLobbyMemberData.charName) - 1);
			MyLobbyMemberData.charName[sizeof(MyLobbyMemberData.charName) - 1] = '\0'; // Ensure null termination

		}

		// Update the lobby member data
		UpdateLobbyMemberData();

		// Update lobby with wether players should remain after match end or be sent home
		if (IsHost()) {
			LocalLobbyData.remainAfterMatch = Settings::modPreferences.remainAfterMatch;
			UpdateLobbyData();
		}
	}

	void P2PLobby::onLobbyDataUpdated(LobbyDataUpdate_t* lobbyDataUpdate) {
		if (_steamMatchMaking == nullptr) {
			printf("steamMatchMaking is null\n");
			return;
		}

		uint64_t lobbyId = lobbyDataUpdate->m_ulSteamIDLobby;
		uint64_t memberId = lobbyDataUpdate->m_ulSteamIDMember;
		bool changed = lobbyDataUpdate->m_bSuccess;

		if (!changed) {
			printf("Lobby data unchanged!\n");
			return;
		}

		const char* data = (lobbyId == memberId)
			? _steamMatchMaking->GetLobbyData(lobbyId, LOBBY_DATA_KEY)
			: _steamMatchMaking->GetLobbyMemberData(lobbyId, memberId, LOBBY_DATA_KEY);

		if (data != nullptr && data[0] != '\0') {
			if (lobbyId == memberId) {
				printf("Handling lobby data...\n");
				handleLobbyDataUpdate(data);
			}
			else {
				printf("Handling lobby member data...\n");
				handleLobbyMemberDataUpdate(memberId, data);
			}
		}
	}

	void P2PLobby::onLobbyCreated(LobbyCreated_t* lobbyCreated) {
	}

	void P2PLobby::onLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure) {
		printf("bIOFailer is: %d\n", bIOFailure);
		if (bIOFailure) return;
		printf("The number of lobbies that matches is: %d\n", pLobbyMatchList->m_nLobbiesMatching);
		int lobbyCount = pLobbyMatchList->m_nLobbiesMatching;
		for (int i = 0; i < lobbyCount - 1; i++) {
			CSteamID lobbyId = _steamMatchMaking->GetLobbyByIndex(i);
			printf("Lobby[%d] with lobby id: %llu\n", i, lobbyId.ConvertToUint64());

			int lobbyDataCount = _steamMatchMaking->GetLobbyDataCount(lobbyId);
			char key[k_nMaxLobbyKeyLength];
			char value[k_cubChatMetadataMax];
				
			for (int j = 0; j < lobbyDataCount - 1; j++) {
				bool bSuccess = SteamMatchmaking()->GetLobbyDataByIndex(lobbyId, i, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax);
				if (bSuccess)
				{
					printf("Lobby Data %d, Key: \"%s\" - Value: \"%s\"\n", i, key, value);
				}
			}
 
		}
	}


	// LobbyUpdate
	void P2PLobby::handleLobbyDataUpdate(const char* data) {

		if (!SyncLobbyData()) return;

		printf("Synchronized lobby data mode is: %d\n", LocalLobbyData.mode);

		switch (LocalLobbyData.mode) {
		case Mode::TeamFight:
			printf("Handling Team Fight Lobby Data...\n");
			handleTeamFightLobbyDataUpdate();
			break;

		case Mode::Brawl:
			printf("Handling Brawl Lobby Data...\n");
			handleBrawlLobbyDataUpdate();
			break;
		}
	}

	void P2PLobby::handleTeamFightLobbyDataUpdate() {
		switch (LocalLobbyData.state) {
		case State::TeamFightInitiate:
			printf("Handling Team Fight Initiate...\n");
			handleTeamFightInitiate();
			break;

		case State::TeamFightOngoing:
			printf("Handling Team Fight Ongoing...\n");
			handleTeamFightOngoing();
			break;

		case State::TeamFightEndMatch:
			printf("Handling Team Fight EndMatch...\n");
			handleTeamFightEndMatch();
			break;
		}
	}

	void P2PLobby::handleTeamFightInitiate() {
		bool isHost = IsHost();
		bool isTeamPicked = _teamFightManager->CheckIfTeamPicked();
		bool isStatePickingTeam = MyLobbyMemberData.state == State::TeamFightPickingTeam;
		bool isStateReadyToStart = MyLobbyMemberData.state == State::TeamFightReadyToStart;

		if (!isTeamPicked && !isStatePickingTeam) {
			MyLobbyMemberData.state = State::TeamFightPickingTeam;
			_teamFightManager->PickingTeam();
			UpdateLobbyMemberData();
		}
		else if (isTeamPicked && !isStateReadyToStart) {
			_netChrsetSyncId = Player::GetPlayerNetChrSyncID();
			_teamFightManager->SetArena(LocalLobbyData.arena);
			MyLobbyMemberData.state = State::TeamFightReadyToStart;
			MyLobbyMemberData.team = Seamless::GetSeamlessInfo().team;
			UpdateLobbyMemberData();
		}

		if (!isTeamPicked) {
			Seamless::ForceUseSeamlessItem(Seamless::JUDICATORS_RULEBOOK_REF);
		}

		if (!isHost) return;

		// Close the lobby to avoid players entering world mid match
		CloseLobby();

		bool isEveryoneReady = CheckIfEveryoneHasState(State::TeamFightReadyToStart);
		printf("Every is ready state: %d\n", isEveryoneReady);
		if (isEveryoneReady) {
			LocalLobbyData.mode = Mode::TeamFight;
			LocalLobbyData.state = State::TeamFightOngoing;
			LocalLobbyData.matchDuration = Settings::modPreferences.matchDuration;
			UpdateLobbyData();
		}
	}

	void P2PLobby::handleTeamFightOngoing() {
		bool isStateOngoing = MyLobbyMemberData.state == State::TeamFightOngoing;

		if (!isStateOngoing && _teamFightManager->StartMatch(LocalLobbyData.matchDuration)) {
			MyLobbyMemberData.state = State::TeamFightOngoing;
			UpdateLobbyMemberData();
		};
	}

	void P2PLobby::handleTeamFightEndMatch() {
		bool hasEndedMatch = MyLobbyMemberData.state == State::TeamFightEndMatch;
		int16_t blueKills = LocalLobbyData.blueTeamScore;
		int16_t redKills = LocalLobbyData.redTeamScore;

		TeamFightManager::Result result = TeamFightManager::STALEMATE;

		if (blueKills > redKills) {
			// Check if the player is in the winning team
			if (MyLobbyMemberData.team == Seamless::BLUE) {
				result = TeamFightManager::VICTORY;
			}
			else {
				result = TeamFightManager::DEFEAT;
			}
		}
		else if (redKills > blueKills) {
			// Check if the player is in the winning team
			if (MyLobbyMemberData.team == Seamless::RED) {
				result = TeamFightManager::VICTORY;
			}
			else {
				result = TeamFightManager::DEFEAT;
			}
		}

		if (!hasEndedMatch && !_teamFightManager->EndMatch(result)) return;
		else {
			MyLobbyMemberData.state = State::TeamFightEndMatch;
			UpdateLobbyMemberData();
		}
		
		if (LocalLobbyData.remainAfterMatch) Warp::WarpTo(Warp::RoundTableHold);
		else Seamless::ForceUseSeamlessItem(Seamless::SEPERATION_MIST_REF);

		if (!IsHost()) return;

		if (CheckIfEveryoneHasState(State::TeamFightEndMatch)) {
			resetLocalData();
			OpenLobby();
			UpdateLobbyData();
		}
	}

	void P2PLobby::handleBrawlLobbyDataUpdate() {
		switch (LocalLobbyData.state) {
		case State::BrawlInitiate: {
			handleBrawlInitiate();
			break;
		}
		case State::BrawlOngoing: {
			handleBrawlOngoing();
			break;
		}
		case State::BrawlEndMatch: {
			handleBrawlEndMatch();
			break;
		}
		}
	}

	void P2PLobby::handleBrawlInitiate() {
		bool isHost = IsHost();
		bool isReadyToStart = MyLobbyMemberData.state == State::BrawlReadyToStart;
		bool isEveryoneReady = CheckIfEveryoneHasState(State::BrawlReadyToStart);

		if (!isReadyToStart) {
			_netChrsetSyncId = Player::GetPlayerNetChrSyncID();
			_brawlManager->SetArena(LocalLobbyData.arena);
			MyLobbyMemberData.state = State::BrawlReadyToStart;
			UpdateLobbyMemberData();
		}

		if (!isHost) return;

		// Close the lobby to avoid players entering world mid match
		CloseLobby();

		if (isEveryoneReady) {
			LocalLobbyData.mode = Mode::Brawl;
			LocalLobbyData.state = State::BrawlOngoing;
			LocalLobbyData.matchDuration = Settings::modPreferences.matchDuration;
			UpdateLobbyData();
		}
	}

	void P2PLobby::handleBrawlOngoing() {
		bool isStateOngoing = MyLobbyMemberData.state == State::BrawlOngoing;
		if (!isStateOngoing && _brawlManager->StartMatch(LocalLobbyData.matchDuration)) {
			MyLobbyMemberData.state = State::BrawlOngoing;
			UpdateLobbyMemberData();
		}
	}

	void P2PLobby::handleBrawlEndMatch() {
		bool hasEndedMatch = MyLobbyMemberData.state == State::BrawlEndMatch;
		int16_t mostKills = LocalLobbyData.mostKills;
		int16_t ownKills = MyLobbyMemberData.kills;

		BrawlManager::Result result = BrawlManager::STALEMATE;

		if (ownKills >= mostKills) {
			uint8_t amountOfPlayerWithSameScore = 0;
			iterateLobbyMembers([&amountOfPlayerWithSameScore, &mostKills](const P2PLobbyMemberData& lobbyMemberData) {
				if (lobbyMemberData.kills == mostKills) amountOfPlayerWithSameScore++;
			});

			if (amountOfPlayerWithSameScore <= 1) {
				result = BrawlManager::VICTORY;
			}
		}
		else {
			result = BrawlManager::DEFEAT;
		}

		if (!hasEndedMatch && !_brawlManager->EndMatch(result)) return;
		
		if(!hasEndedMatch){
			MyLobbyMemberData.state = State::BrawlEndMatch;
			UpdateLobbyMemberData();
		}

		if (LocalLobbyData.remainAfterMatch) Warp::WarpTo(Warp::RoundTableHold);
		else Seamless::ForceUseSeamlessItem(Seamless::SEPERATION_MIST_REF);

		if (!IsHost()) return;

		if (CheckIfEveryoneHasState(State::BrawlEndMatch)) {
			resetLocalData();
			OpenLobby();
			UpdateLobbyData();
		}
	}


	// LobbyMemeberUpdate
	void P2PLobby::handleLobbyMemberDataUpdate(uint64_t memberId, const char* data) {
		P2PLobbyMemberData lobbyMemberData;
		readLobbyMemberData(memberId, [&lobbyMemberData](const P2PLobbyMemberData& lbd) {
			lobbyMemberData = lbd;
		});

		switch (LocalLobbyData.mode) {

		case Mode::TeamFight: {
			handleTeamFightLobbyMemberDataUpdate(memberId, &lobbyMemberData);
		}

		case Mode::Brawl: {
			handleBrawlLobbyDataMemberUpdate(memberId, &lobbyMemberData);
		}

		}
	}

	void P2PLobby::handleTeamFightLobbyMemberDataUpdate(uint64_t memberId, P2PLobbyMemberData* lobbyMemberData) {
		if (IsHost())  recalculateAndUpdateTeamFightScore();
	}

	void P2PLobby::handleBrawlLobbyDataMemberUpdate(uint64_t memberId, P2PLobbyMemberData* lobbyMemberData) {
		if (IsHost()) recalculateAndUpdateMostKills(memberId);
	}
}
