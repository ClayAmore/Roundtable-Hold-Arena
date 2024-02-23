#pragma once

#include <d3d11.h>
#include <vector>
#include <fstream>
#include <WICTextureLoader.h>
#include <comdef.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <wrl/client.h>
#include <chrono>
#include <string>

#include "Logger.h"

#undef DrawText

namespace OF
{
	enum HorizontalAlignment {
		Left,
		HCenter,
		Right
	};

	enum VerticalAlignment {
		Top,
		VCenter,
		Bottom
	};

	static struct Box
	{
		float z = 0.0f;
		int width = 0;
		int height = 0;
		bool pressed = false;
		bool clicked = false;
		bool hover = false;
		bool draggable = true;
		bool hasBeenRendered = false;
		Box* parentBox = nullptr;
		RECT position = { 0, 0, 0, 0 };
		RECT margin = { 0, 0, 0, 0 };
		HorizontalAlignment horizontalAlignment = Left;
		VerticalAlignment verticalAlignment = Top;
	};

	static Logger logger{ "OverlayFramework" };
	static HWND ofWindow = 0;
	static int ofWindowWidth = 0;
	static int ofWindowHeight = 0;
	static std::vector<Box*> ofBoxes = std::vector<Box*>();
	static constexpr unsigned char UNBOUND = 0x07;
	static std::string dllPath;

	static Microsoft::WRL::ComPtr<ID3D11Device> ofDevice = nullptr;
	static std::shared_ptr<DirectX::SpriteBatch> ofSpriteBatch = nullptr;
	static std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> ofTextures = std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
	static std::vector<std::shared_ptr<DirectX::SpriteFont>> ofFonts = std::vector<std::shared_ptr<DirectX::SpriteFont>>();
	static std::shared_ptr<DirectX::SpriteFont> ofActiveFont = nullptr;

	// Gives the framework the required DirectX objects to draw
	static void InitFramework(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		std::shared_ptr<DirectX::SpriteBatch> spriteBatch,
		HWND window, std::string path)
	{
		logger.Log("Initialized");
		ofDevice = device;
		ofSpriteBatch = spriteBatch;
		ofWindow = window;

		RECT hwndRect;
		GetClientRect(ofWindow, &hwndRect);
		ofWindowWidth = hwndRect.right - hwndRect.left;
		ofWindowHeight = hwndRect.bottom - hwndRect.top;

		dllPath = path;
	}

	static int MapIntToRange(
		int number,
		int inputStart,
		int inputEnd,
		int outputStart,
		int outputEnd)
	{
		return outputStart + (outputEnd - outputStart) * (number - inputStart) / (inputEnd - inputStart);
	}

	static float MapFloatToRange(
		float number,
		float inputStart,
		float inputEnd,
		float outputStart,
		float outputEnd)
	{
		return outputStart + (outputEnd - outputStart) * (number - inputStart) / (inputEnd - inputStart);
	}

	static int LoadTexture(std::string filepath)
	{
		if (ofDevice.Get() == nullptr)
		{
			logger.Log("Could not load texture, ofDevice is nullptr! Run InitFramework before attempting to load textures!");
			return -1;
		}

		if (ofTextures.size() == 0 && filepath != "blank") {
			if (LoadTexture("blank") != 0)
			{
				return -1;
			}
		}
		else if (filepath == "blank")
		{
			filepath = dllPath + "\\hook_textures\\blank.jpg";
		}

		logger.Log("Loading texture: %s", filepath.c_str());

		std::wstring wideString(filepath.length(), ' ');
		std::copy(filepath.begin(), filepath.end(), wideString.begin());
		std::fstream file = std::fstream(filepath);
		if (file.fail())
		{
			logger.Log("Texture loading failed, file not found: %s", filepath.c_str());
			file.close();
			return -1;
		}
		file.close();

		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			logger.Log("Error %#010x when initializing the COM library", hr);
		}
		else
		{
			logger.Log("Successfully initialized the COM library");
		}

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
		HRESULT texResult = DirectX::CreateWICTextureFromFile(ofDevice.Get(), wideString.c_str(), nullptr, texture.GetAddressOf());

		_com_error texErr(texResult);
		logger.Log("Texture HRESULT: %s", texErr.ErrorMessage());
		if (FAILED(texResult))
		{
			logger.Log("Texture loading failed: %s", filepath.c_str());
			return -1;
		}

		ofTextures.push_back(texture);
		return ofTextures.size() - 1;
	}

	static int LoadFont(std::string filepath)
	{
		if (ofDevice.Get() == nullptr)
		{
			logger.Log("Could not load font, ofDevice is nullptr! Run InitFramework before attempting to load fonts!");
			return -1;
		}

		logger.Log("Loading font: %s", filepath.c_str());

		std::fstream file = std::fstream(filepath);
		std::wstring wideString(filepath.length(), ' ');
		std::copy(filepath.begin(), filepath.end(), wideString.begin());
		if (file.fail())
		{
			logger.Log("Font loading failed: %s", filepath.c_str());
			file.close();
			return -1;
		}

		file.close();

		logger.Log("Font was loaded successfully");
		ofFonts.push_back(std::make_shared<DirectX::SpriteFont>(ofDevice.Get(), wideString.c_str()));

		return ofFonts.size() - 1;
	}

	static void SetFont(int font)
	{
		if (font > ofFonts.size() - 1 || font < 0)
		{
			logger.Log("Attempted to set invalid font!");
			return;
		}

		ofActiveFont = ofFonts[font];
	}

	static void PlaceOnTop(Box* box)
	{
		static std::vector<int> ofBoxOrder = std::vector<int>();
		size_t boxIndex = 0;
		for (size_t i = 0; i < ofBoxes.size(); i++)
		{
			if (ofBoxes[i] == box)
			{
				boxIndex = i;
				break;
			}
		}

		ofBoxOrder.push_back(boxIndex);
		for (size_t i = 0; i < ofBoxOrder.size() - 1; i++)
		{
			if (ofBoxes[ofBoxOrder[i]] == ofBoxes[ofBoxOrder.back()])
			{
				ofBoxOrder.erase(ofBoxOrder.begin() + i);
			}
		}

		for (float i = 0; i < ofBoxOrder.size(); i++)
		{
			ofBoxes[ofBoxOrder[i]]->z = 1.0f / (1 + (i / 1000));
		}
	}

	static RECT GetParentRect(Box* box)
	{
		if (box == nullptr)
		{
			return { 0, 0, 0, 0 };
		}

		RECT parentRect;

		if (box->parentBox != nullptr) {
			parentRect = box->parentBox->position;
		}
		else {
			GetClientRect(ofWindow, &parentRect);
		}

		return parentRect;
	}

	static Box* CreateBox(Box* parentBox, int width, int height, HorizontalAlignment horizontalAlignment = Left, VerticalAlignment verticalAlignment = Top, RECT margin = { 0,0,0,0 })
	{
		Box* box = new Box;
		box->horizontalAlignment = horizontalAlignment;
		box->verticalAlignment = verticalAlignment;
		box->width = width;
		box->height = height;
		box->margin = margin;
		box->parentBox = parentBox;

		if (parentBox != nullptr)
		{
			box->draggable = false;
		}

		ofBoxes.push_back(box);
		PlaceOnTop(box);
		return ofBoxes.back();
	}

	static Box* CreateBox(int width, int height, HorizontalAlignment horizontalAlignment = Left, VerticalAlignment verticalAlignment = Top, RECT margin = { 0,0,0,0 })
	{
		return CreateBox(nullptr, width, height, horizontalAlignment, verticalAlignment, margin);
	}

	static void _DrawBox(
		Box* box,
		DirectX::XMVECTOR color,
		int textureID,
		float scale = 1.0f
	)
	{
		static bool ofFailedToLoadBlank = false;

		if (box == nullptr)
		{
			logger.Log("Attempted to render a nullptr Box!");
			return;
		}

		if (ofSpriteBatch == nullptr)
		{
			logger.Log("Attempted to render with ofSpriteBatch as nullptr! Run InitFramework before attempting to draw!");
			return;
		}

		if (ofTextures.size() < 1)
		{
			if (ofFailedToLoadBlank == false)
			{
				if (LoadTexture("blank") != 0)
				{
					ofFailedToLoadBlank = true;
					return;
				}
			}
			else
			{
				return;
			}
		}

		if (textureID < 0 || textureID > ofTextures.size() - 1)
		{
			logger.Log("'%i' is an invalid texture ID!", textureID);
			return;
		}

		RECT parentRect = GetParentRect(box);

		int horizontalMargin = box->margin.left - box->margin.right;
		int verticalMargin = box->margin.top - box->margin.bottom;

		RECT rect;
		rect.left = parentRect.left + horizontalMargin * scale;
		rect.right = parentRect.left + (box->width + horizontalMargin) * scale;

		rect.top = parentRect.top + verticalMargin * scale;
		rect.bottom = parentRect.top + (box->height + verticalMargin) * scale;


		if (box->horizontalAlignment == HCenter) {
			rect.left = parentRect.right * 0.5 - (box->width * 0.5 + horizontalMargin) * scale;
			rect.right = parentRect.right * 0.5 + (box->width * 0.5 + horizontalMargin) * scale;
		}
		else if (box->horizontalAlignment == Right) {
			rect.left = parentRect.right - (box->width - horizontalMargin) * scale;
			rect.right = parentRect.right + horizontalMargin * scale;
		}

		if (box->verticalAlignment == VCenter) {
			rect.top = parentRect.bottom * 0.5 - (box->height * 0.5 + verticalMargin) * scale;
			rect.bottom = parentRect.bottom * 0.5 + (box->height * 0.5 + verticalMargin) * scale;
		}
		else if (box->verticalAlignment == Bottom) {
			rect.top = parentRect.bottom - (box->height - verticalMargin) * scale;
			rect.bottom = parentRect.bottom + verticalMargin * scale;
		}

		box->position = rect;

		box->hasBeenRendered = true;
		ofSpriteBatch->Draw(ofTextures[textureID].Get(), rect, nullptr, color, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::SpriteEffects_None, box->z);
	}

	static void DrawBox(
		Box* box,
		int textureID,
		float scale = 1.0f
	)
	{
		_DrawBox(box, { 1.0f, 1.0f, 1.0f, 1.0f }, textureID, scale);
	}

	static void DrawBox(
		Box* box,
		int r,
		int g,
		int b,
		int a = 255,
		float scale = 1.0f
	)
	{
		float _r = MapFloatToRange((float)r, 0.0f, 255.0f, 0.0f, 1.0f);
		float _g = MapFloatToRange((float)g, 0.0f, 255.0f, 0.0f, 1.0f);
		float _b = MapFloatToRange((float)b, 0.0f, 255.0f, 0.0f, 1.0f);
		float _a = MapFloatToRange((float)a, 0.0f, 255.0f, 0.0f, 1.0f);
		_DrawBox(box, { _r, _g, _b, _a }, 0, scale);
	}


	static void DrawText(
		Box* box,
		std::string text,
		float scale = 1.0f,
		int r = 255,
		int g = 255,
		int b = 255,
		int a = 255,
		float rotation = 0.0f,
		HorizontalAlignment horizontalAlignment = Left,
		VerticalAlignment verticalAlignment = Top,
		RECT margin = { 0, 0, 0, 0 })
	{
		if (ofActiveFont == nullptr)
		{
			logger.Log("Attempted to render text with an invalid font, make sure to run SetFont first!");
			return;
		}

		RECT position = box->position;

		DirectX::XMVECTOR result = ofActiveFont->MeasureString(text.c_str(), false);
		float textWidth = DirectX::XMVectorGetX(result) * scale;
		float textHeight = DirectX::XMVectorGetY(result) * scale;

		float horizontalMargin = (margin.left - margin.right) * scale;
		float verticalMargin = (margin.top - margin.bottom) * scale;

		float textPositionX = box->position.left + horizontalMargin;
		float textPositionY = box->position.top + verticalMargin;

		float parentWidth = (float)(position.right - position.left);
		float parentHeight = (float)(position.bottom - position.top);

		if (horizontalAlignment == HCenter) {
			textPositionX += (parentWidth) * 0.5f - (textWidth * 0.5f - horizontalMargin);
		}
		else if (horizontalAlignment == Right) {
			textPositionX += (parentWidth)-textWidth + horizontalMargin;
		}

		if (verticalAlignment == VCenter) {
			textPositionY += (parentHeight) * 0.5f - (textHeight * 0.5f - verticalMargin);
		}
		else if (verticalAlignment == Bottom) {
			textPositionY += (parentHeight)-textHeight + verticalMargin;
		}

		DirectX::XMFLOAT2 textPos = DirectX::XMFLOAT2
		(
			textPositionX,
			textPositionY
		);

		float _r = MapFloatToRange((float)r, 0.0f, 255.0f, 0.0f, 1.0f);
		float _g = MapFloatToRange((float)g, 0.0f, 255.0f, 0.0f, 1.0f);
		float _b = MapFloatToRange((float)b, 0.0f, 255.0f, 0.0f, 1.0f);
		float _a = MapFloatToRange((float)a, 0.0f, 255.0f, 0.0f, 1.0f);

		ofActiveFont->DrawString(
			ofSpriteBatch.get(),
			text.c_str(),
			textPos,
			{ _r, _g, _b, _a },
			rotation,
			{ 0.0f, 0.0f },
			scale,
			DirectX::SpriteEffects_None,
			box->z);
	}

	static bool CheckHotkey(unsigned char key, unsigned char modifier = UNBOUND)
	{
		static std::vector<unsigned char> notReleasedKeys;

		if (ofWindow != GetForegroundWindow())
		{
			return false;
		}

		bool keyPressed = GetAsyncKeyState(key) & 0x8000;
		bool modifierPressed = GetAsyncKeyState(modifier) & 0x8000;

		if (key == UNBOUND)
		{
			return modifierPressed;
		}

		auto iterator = std::find(notReleasedKeys.begin(), notReleasedKeys.end(), key);
		bool keyNotReleased = iterator != notReleasedKeys.end();

		if (keyPressed && keyNotReleased)
		{
			return false;
		}

		if (!keyPressed)
		{
			if (keyNotReleased)
			{
				notReleasedKeys.erase(iterator);
			}
			return false;
		}

		if (modifier != UNBOUND && !modifierPressed)
		{
			return false;
		}

		notReleasedKeys.push_back(key);
		return true;
	}
};