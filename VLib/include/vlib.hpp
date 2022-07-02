#define WIN32_LEAN_AND_MEAN 
#define GEN_COMBINE(x, y, z, i) x ## y ## z ## i
#define COMBINE(x, y, z, i) GEN_COMBINE(x, y, z, i)
#define PAD(bytes) private: char COMBINE( __pad_, __COUNTER__, _, __LINE__ ) [bytes]; public:

//#include <chaiscript/chaiscript.hpp>

#include <Windows.h>
#include <system_error>

extern LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

namespace vlib {

	struct WindowRect {
		int m_x, m_y, m_w, m_h;
		WindowRect(const int& x = CW_USEDEFAULT, const int& y = CW_USEDEFAULT, const int& w = CW_USEDEFAULT, const int& h = CW_USEDEFAULT) : m_x{ x }, m_y{ y }, m_w{ w }, m_h{ h } {}
	};

	struct ProcessInfo {
		HINSTANCE m_instance;
		const char* m_class_name;
		const char* m_window_name;
		int m_flags;
		int m_extended_flags;
		WindowRect m_window_rect;
		int m_icon;
		ProcessInfo(const HINSTANCE& instance, const char* class_name = "cls_vlib", const char* window_name = "wnd_vlib", const int& flags = WS_OVERLAPPEDWINDOW | WS_VISIBLE, const int& extended_flags = WS_EX_LAYERED, const WindowRect& window_rect = {}, const int& icon = 0)
			: m_instance{ instance }, m_class_name{ class_name }, m_window_name{ window_name }, m_flags{ flags }, m_extended_flags{ extended_flags }, m_window_rect{ window_rect }, m_icon{ icon } {}
	};

	class VProcess {
	private:
		WNDCLASSEX p_class_info;
		HWND p_window_handle;
	public:
		VProcess(const ProcessInfo& info);
		const HWND& GetWindowHandle() const {

			return p_window_handle;
		}
	};
}