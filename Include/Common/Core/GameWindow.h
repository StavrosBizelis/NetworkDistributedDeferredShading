#pragma once

#include <windows.h>
#include <string>
#include "glm/glm.hpp"
#include "Common/Core/MyUtilities.h"
LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam);

class GameWindow {
public:
	static GameWindow& GetInstance();
	GameWindow();

	HDC Init(HINSTANCE hinstance, const glm::vec2& a_dimensions); ///< OpenGL
	HDC Init(HINSTANCE hinstance, const glm::vec2& a_dimensions, void* a_vkInstance, void* a_mvkSurface);  ///< Vulkan
	void Deinit();

	void SetDimensions(RECT dimensions) {m_dimensions = dimensions;}
	RECT GetDimensions() {return m_dimensions;}

	bool Fullscreen() const { return m_fullscreen; }

	HDC Hdc() const { return m_hdc; }
	HINSTANCE Hinstance() const { return m_hinstance; }
	HGLRC Hrc() const { return m_hrc; }
	HWND  Hwnd() const { return m_hwnd; }

private:
	
	GameWindow(const GameWindow&);
	void operator=(const GameWindow&);

	void CreateGameWindow(std::string title, const glm::vec2& a_dimensions);
	void InitOpenGL();
	bool InitGLEW();
	void InitVulkan();
	void RegisterSimpleOpenGLClass(HINSTANCE hInstance);

	bool  m_fullscreen;

	HDC   m_hdc;
	HINSTANCE m_hinstance;
	HGLRC m_hrc;
	HWND  m_hwnd;

	LPSTR m_class;
	RECT  m_dimensions;

	std::string m_appName;

  void* m_vkInstance;
  void* m_vkSurface;
  ImplTech m_tech;
};