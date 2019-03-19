//************************************************************
//		本文件为播放器类的定义，为实现播放器的主要代码
//************************************************************
#pragma once
#pragma comment (lib,"winmm.lib")
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include<tuple>
#include<deque>
#include<iostream>
#include<algorithm>
#include <VersionHelpers.h>
#include<iomanip>
#include"Common.h"
#include"Console.h"
#include"Lyrics.h"
using std::ofstream;
using std::ifstream;
using std::string;
using std::wstring;
using std::vector;
using std::deque;

using PathInfo = std::tuple<wstring, int, int>;		//储存路径信息
const size_t PATH{ 0 }, TRACK{ 1 }, POSITION{ 2 };		//定义用于表示tuple<>对象中的3个字段的常量

class CPlayer
{
private:
	vector<wstring> m_playlist;		//播放列表，储存音乐文件的文件名
	vector<Time> m_all_song_length;		//储存每个音乐文件的长度
	wstring m_path;		//当前播放文件的路径
	wstring m_lyric_path;	//歌词文件夹的路径
	wstring m_current_file_name;		//正在播放的文件名
	deque<PathInfo> m_recent_path;		//最近打开过的路径

	Time m_song_length;		//正在播放的文件的长度
	Time m_current_position;		//当前播放到的位置
	int m_song_length_int;		//正在播放的文件的长度（int类型）
	int m_current_position_int;		//当前播放到的位置（int类型）

	int m_index{ 0 };	//当前播放音乐的序号
	int m_song_num{ 0 };	//播放列表中的歌曲总数
	MCIERROR m_error_code{ 0 };
	int m_playing{ 0 };		//正在播放标志（0：已停止，1：已暂停，2：正在播放）
	int m_repeat_mode{ 0 };		//循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
	int m_volume{ 100 };		//音量（百分比）

	int m_display_page{ 1 };		//当前显示的播放列表的页
	int m_total_page;		//播放列表的总页数
	int m_song_per_page;		//播放列表每页显示几首歌曲
	
	CLyrics m_Lyrics;		//歌词

	int m_width;		//窗口宽度
	int m_hight;		//窗口高度
	
	wchar_t m_font[32];		//控制台字体（字体名称必须以宽字符表示）
	int m_font_size;	//字体大小

	wstring m_config_path;	//配置文件的路径
	wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径

	vector<int> m_find_result;		//储存查找结果的歌曲序号

	void IniPlayList(bool cmd_para = false);	//初始化播放列表(如果参数为true，表示从命令行直接获取歌曲文件，而不是从指定路径下搜索)
	void IniConsole();		//初始化控制台

	void ChangePath(const wstring& path, int track = 0);		//改变当前路径

	void LoadRecentPath();		//从文件载入最近路径列表
	void SaveRecentPath() const;		//将最近路径列表保存到文件
	void EmplaceCurrentPathToRecent();		//将当前路径插入到最近路径中

	void FindFile(const wstring& key_word);		//根据参数中的关键字查找文件，将结果保存在m_find_result中

public:
	CPlayer();
	void Create();		//构造CPlayer类
	void Create(const vector<wstring>& files);	//构造CPlayer类
	void MusicControl(Command command);		//控制音乐播放
	bool SongIsOver() const;		//判断当前音乐是否播放完毕
	void GetCurrentPosition();		//获取当前播放到的位置
	void GetSongLength();		//获取正在播放文件的长度

	void ShowInfo() const;		//显示歌曲信息
	void ShowPlaylist() const;		//显示播放列表
	void ShowProgressBar() const;		//显示进度条
	void ShowLyrics(bool force_refresh = false) const;		//显示歌词
	void ShowLyricsSingleLine() const;		//显示歌词（单行模式）
	void ShowLyricsMultiLine(bool force_refresh = false) const;		//显示歌词（多行模式）
	void SwitchPlaylist(int iperation);		//播放列表翻页
	int GetCurrentSecond();		//获取当前播放到的位置的秒数（用于使界面每1秒刷新一次）
	bool IsPlaying() const;			//判断当前是否正在播放

	bool PlayTrack(int song_track);		//播放指定序号的歌曲
	
	void SetVolume();		//用m_volume的值设置音量
	void SetPath();		//设置路径
	void SetTrack();		//更改正在播放的歌曲
	void SetRepeatMode();		//更改循环模式

	bool ErrorDispose();		//错误处理（已消除错误返回true，否则返回false）

	void SetTitle() const;		//设置窗口标题
	
	void SaveConfig() const;		//保存配置到ini文件
	void LoadConfig();		//从ini文件读取配置

	void IniLyrics();		//初始化歌词

	void GetWindowsSize(int width, int hight);		//获取窗口宽度和高度
	//void GetConsoleSetting();		//获取控制台中的字体和字体大小的设置

	void ExplorePath() const;	//用资源管理器打开当前路径
	void Find();			//查找文件
};


inline CPlayer::CPlayer()
{
	m_config_path = GetExePath() + L"config.ini";
	m_recent_path_dat_path = GetExePath() + L"recent_path.dat";
}

inline void CPlayer::Create()
{
	LoadConfig();
	IniConsole();
	LoadRecentPath();
	EmplaceCurrentPathToRecent();
	IniPlayList();	//初始化播放列表
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

inline void CPlayer::Create(const vector<wstring>& files)
{
	LoadConfig();
	IniConsole();
	LoadRecentPath();
	size_t index;
	index = files[0].find_last_of(L'\\');
	m_path = files[0].substr(0, index + 1);
	EmplaceCurrentPathToRecent();
	for (const auto& file : files)
	{
		index = file.find_last_of(L'\\');
		m_playlist.push_back(file.substr(index + 1));
	}
	IniPlayList(true);
	m_current_position_int = 0;
	m_current_position = { 0,0,0 };
	m_index = 0;
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::IniConsole()
{
	char buff[32];
	sprintf_s(buff, sizeof(buff), "mode con:cols=%d lines=%d", m_width, m_hight);
	system(buff);		//设置窗口的宽度和高度
	//系统在windows10以下时，在初始化时设置一个固定的缓冲区大小。
	//（windows10可以根据窗口大小自动调整缓冲区大小。）
	if (!IsWindows8OrGreater())
	{
		COORD size{300, 80};
		SetConsoleScreenBufferSize(handle, size);		//设置窗口缓冲区大小
	}
	
	//设置字体
	CONSOLE_FONT_INFOEX info{ 0 };
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = m_font_size; 		//保持X为0  
	info.FontWeight = FW_NORMAL;
	wcscpy_s(info.FaceName, m_font);
	SetCurrentConsoleFontEx(handle, NULL, &info); 

	CursorVisible(false);		//隐藏光标

	setlocale(LC_ALL, "");		//将区域设置设定为从操作系统获得的用户默认的ANSI代码页

}

void CPlayer::IniPlayList(bool cmd_para)
{
	if (!cmd_para)
	{
		vector<wstring> file_fromat{ L"mp3", L"wma", L"wav", L"mid" };
		GetAllFormatFiles(m_path, m_playlist, file_fromat, 2997);
	}
	std::sort(m_playlist.begin(), m_playlist.end());		//对播放列表按名称排序
	//m_index = 0;
	m_song_num = m_playlist.size();
	m_song_per_page = m_hight - 7;
	//计算播放列表要显示的总页数
	if (m_song_num == 0)
		m_total_page = 1;
	else
		m_total_page = (m_song_num - 1) / m_song_per_page + 1;
	m_display_page = m_index / m_song_per_page + 1;		//设定播放列表中当前显示的页
	if (m_index >= m_song_num) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数

	system("cls");
	//获取播放列表中每一首歌曲的长度
	wchar_t buff[32];
	swprintf_s(buff, sizeof(buff) / 2, L"找到%d首歌曲，正在生成播放列表，请稍候……", m_song_num);
	PrintWString(buff, 0, 0, DARK_WHITE);
	m_all_song_length.clear();
	m_all_song_length.resize(m_song_num);
	//最多只获取MAX_NUM_LENGTH首歌的长度，超过MAX_NUM_LENGTH数量的歌曲的长度在打开时获得。防止文件夹中音频文件过多导致等待时间过长
	int start;			//初始化时从第几首歌曲开始获取歌曲长度
	int count{ 0 };		//获取到歌曲长度的文件的数量
	if (m_song_num <= MAX_NUM_LENGTH)
		start = 0;
	else
		start = (m_display_page - 1)*m_song_per_page;	//如果播放列表时可歌曲数量超过了MAX_NUM_LENGTH，就从播放列表显示的当前页的第一首歌曲开始获取歌曲长度
	for (int i{ start }; i < m_song_num && count < MAX_NUM_LENGTH; i++, count++)
	{
		m_current_file_name = m_playlist[i];
		if (!FileIsMidi(m_current_file_name))		//不获取MIDI文件的长度（MIDI文件的长度在打开时获得）
		{
			//MusicControl(Command::OPEN);
			m_error_code = mciSendStringW((L"open \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);
			GetSongLength();
			m_all_song_length[i] = m_song_length;
			MusicControl(Command::CLOSE);
		}
	}

	system("cls");

	m_song_length = {0,0,0};
	//m_current_position = {0,0,0};
	if (m_song_num==0)
		m_current_file_name = L"没有找到文件";
	else
		m_current_file_name = m_playlist[m_index];
}

void CPlayer::IniLyrics()
{
	wstring lyric_path{ m_path + m_current_file_name };		//得到路径+文件名的字符串
	lyric_path.replace(lyric_path.size() - 3, 3, L"lrc");		//将最后3个字符的扩展名替换成lrc
	if (FileExist(lyric_path))
	{
		if (!FileIsMidi(m_current_file_name))		//当前歌曲不是midi音乐才初始化歌词
			m_Lyrics = CLyrics{ lyric_path };
		else
			m_Lyrics = CLyrics{};
	}
	else		//当前目录下没有对应的歌词文件时，就在m_lyric_path目录下寻找歌词文件
	{
		lyric_path = m_lyric_path + m_current_file_name;
		lyric_path.replace(lyric_path.size() - 3, 3, L"lrc");
		if(!FileIsMidi(m_current_file_name) && FileExist(lyric_path))
			m_Lyrics = CLyrics{ lyric_path };
		else
			m_Lyrics = CLyrics{};
	}
}

void CPlayer::MusicControl(Command command)
{
	wchar_t buff[16];
	switch (command)
	{
	case Command::OPEN: 
		m_error_code = mciSendStringW((L"open \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);
		//为了能播放路径中有空格的文件，在路径的前后各加上一个双引号
		//GetCurrentPosition();
		GetSongLength();
		if (m_song_num > 0) m_all_song_length[m_index] = m_song_length;		//打开文件后再次将获取的文件长度保存到m_all_song_length容器中
		break;
	case Command::PLAY: m_error_code = mciSendStringW((L"play \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0); m_playing = 2; break;
	case Command::CLOSE: m_error_code = mciSendStringW((L"close \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0); m_playing = 0; break;
	case Command::PAUSE: m_error_code = mciSendStringW((L"pause \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0); m_playing = 1; break;
	case Command::STOP:
		m_error_code = mciSendStringW((L"stop \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);
		m_playing = 0;
		m_error_code = mciSendStringW((L"seek \"" + m_path + m_current_file_name + L"\" to 0").c_str(), NULL, 0, 0);		//停止后定位到0位置
		GetCurrentPosition();
		break;
	case Command::FF:		//快进
		GetCurrentPosition();		//获取当前位置（毫秒）
		if (!FileIsMidi(m_current_file_name))
		{
			m_current_position_int += 5000;		//不是MIDI音乐时每次快进5000毫秒
			if (m_current_position_int > m_song_length_int) m_current_position_int -= 5000;
		}
		else
		{
			m_current_position_int += 30;		//MIDI音乐时每次快进30个位置
			if (m_current_position_int > m_song_length_int) m_current_position_int -= 30;
		}
		_itow_s(m_current_position_int, buff, 10);
		m_error_code = mciSendStringW((L"seek \"" + m_path + m_current_file_name + L"\" to " + buff).c_str(), NULL, 0, 0);		//定位到新的位置
		if (m_playing == 2)
			m_error_code = mciSendStringW((L"play \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);		//继续播放
		break;
	case Command::REW:		//快退
		GetCurrentPosition();		//获取当前位置（毫秒）
		if (!FileIsMidi(m_current_file_name))
			m_current_position_int -= 5000;		//不是MIDI音乐时每次快退5000毫秒
		else
			m_current_position_int -= 30;		//MIDI音乐时每次快退30个位置
		if(m_current_position_int < 0) m_current_position_int = 0;		//防止快退到负的位置
		_itow_s(m_current_position_int, buff, 10);
		m_error_code = mciSendStringW((L"seek \"" + m_path + m_current_file_name + L"\" to " + buff).c_str(), NULL, 0, 0);		//定位到新的位置
		if (m_playing == 2)
			m_error_code = mciSendStringW((L"play \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);		//继续播放
		break;
	case Command::PLAY_PAUSE:
		if (m_playing == 2)
		{
			m_error_code = mciSendStringW((L"pause \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);
			m_playing = 1;
		}
		else
		{
			m_error_code = mciSendStringW((L"play \"" + m_path + m_current_file_name + L"\"").c_str(), NULL, 0, 0);
			m_playing = 2;
		}
		break;
	case Command::VOLUME_UP:
		if (m_volume < 100 && !FileIsMidi(m_current_file_name))		//如果播放的是midi音乐则不允许调整音量（MCI不支持调整MIDI音乐的音量）
		{
			m_volume+=2;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::VOLUME_DOWN:
		if (m_volume > 0 && !FileIsMidi(m_current_file_name))
		{
			m_volume-=2;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::SEEK:		//定位到m_current_position的位置
		if (m_current_position_int > m_song_length_int)
		{
			m_current_position_int = 0;
			m_current_position = Time{ 0, 0, 0 };
		}
		_itow_s(m_current_position_int, buff, 10);
		m_error_code = mciSendStringW((L"seek \"" + m_path + m_current_file_name + L"\" to " + buff).c_str(), NULL, 0, 0);		//定位到新的位置
		break;
	default: break;
	}
}

bool CPlayer::SongIsOver() const
{
	//GetCurrentPosition();
	if (m_song_length_int > m_current_position_int)
		return false;
	else return true;
}

void CPlayer::GetSongLength()
{
	wchar_t buff[16];
	m_error_code = mciSendStringW((L"status \"" + m_path + m_current_file_name + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
	m_song_length_int = _wtoi(buff);		//将获得的长度转换成int类型
	m_song_length = int2time(m_song_length_int);		//将长度转换成Time结构
}

void CPlayer::GetCurrentPosition()
{
	wchar_t buff[16];
	m_error_code = mciSendStringW((L"status \"" + m_path + m_current_file_name + L"\" position").c_str(), buff, 15, 0);
	m_current_position_int = _wtoi(buff);
	m_current_position = int2time(m_current_position_int);
}

/* int CPlayer::GetVolume()
{
	char buff[16];
	int volume;
	mciSendStringW(("status \"" + m_path + m_current_file_name + "\" volume").c_str(), buff, 15, 0);
	volume = atoi(buff);
	m_volume = volume;
	return m_volume;
}*/
 
void CPlayer::SetVolume()
{
	wchar_t buff[16];
	_itow_s(m_volume*10, buff, 10);		//设置音量100%时为1000
	m_error_code = mciSendStringW((L"setaudio \"" + m_path + m_current_file_name + L"\" volume to " + buff).c_str(), NULL, 0, 0);
}
 
void CPlayer::ShowInfo() const
{
	//显示播放状态
	int song_name_length{ m_width - 25 };		//歌曲标题显示的半角字符数
	switch (m_playing)
	{
	case 1: PrintWString(L"已暂停  ", 0, 0, WHITE); break;
	case 2: PrintWString(L"正在播放", 0, 0, WHITE); break;
	default: PrintWString(L"已停止  ", 0, 0, WHITE); break;
	}
	ClearString(14, 0, song_name_length);		//清除标题处的字符
	//显示正在播放的歌曲序号
	wchar_t buff[32];
	swprintf_s(buff, sizeof(buff)/2, L"%.3d", m_index + 1);
	PrintWString(buff, 10, 0, DARK_CYAN);
	//显示正在播放的歌曲名称
	wstring temp;
	static int name_start{ 0 };
	static wstring last_file_name;
	if (m_current_file_name != last_file_name)		//如果正在播放的歌曲发生了切换，就要把name_start清零
	{
		name_start = 0;
		last_file_name = m_current_file_name;
	}
	if (WcharStrHalfWidthLen(m_current_file_name.c_str()) <= song_name_length)		//当文件名的长度不超过song_name_length个字符时，直接显示出来
	{
		PrintWString(m_current_file_name.c_str(), 14, 0, song_name_length, CYAN);		//输出正在播放的文件名（最多只输出song_name_length个字符）
	}
	else		//文件名长度超过song_name_length个字符时滚动显示，此函数每调用一次滚动一次
	{
		temp = m_current_file_name.substr(name_start, song_name_length);		//取得第i个开始的song_name_length个字符
		name_start++;
		if (name_start > WcharStrHalfWidthLen(m_current_file_name.c_str()) - song_name_length - FullWidthCount(m_current_file_name.c_str(), name_start))
			name_start = 0;
		PrintWString(temp.c_str(), 14, 0, song_name_length, CYAN);
	}
	
	PrintWString(L"音量：",m_width - 9, 0, RED);
	ClearString(m_width - 3, 0, 3);
	PrintInt(m_volume, m_width - 3, 0, RED);
	
	if (wcscmp(m_font, L"Microsoft YaHei Mono") == 0)
		PrintWString(L"播放/暂停(空格) 停止(S) 上一曲(V) 下一曲(N) 快退/快进(<-->) 设置路径(T) 跳转(K) 音量(上下) 浏览文件(E) 查找(F) 退出(ESC)", 0, 1, m_width, YELLOW);
	else
		PrintWString(L"播放/暂停(空格) 停止(S) 上一曲(V) 下一曲(N) 快退/快进(←→) 设置路径(T) 跳转(K) 音量(↑↓) 浏览文件(E) 查找(F) 退出(ESC)", 0, 1,m_width, YELLOW);

	PrintWString(L"循环模式(M):", m_width - 20, 4, GREEN);
	switch (m_repeat_mode)
	{
	case 0: PrintWString(L"顺序播放", m_width - 8, 4, GREEN); break;
	case 1: PrintWString(L"随机播放", m_width - 8, 4, GREEN); break;
	case 2: PrintWString(L"列表循环", m_width - 8, 4, GREEN); break;
	case 3: PrintWString(L"单曲循环", m_width - 8, 4, GREEN); break;
	default: break;
	}

	swprintf_s(buff, sizeof(buff)/2, L"MusicPlayer V%s 作者：ZY", VERSION);
	PrintWString(buff, 0, m_hight - 1,m_width - 37, GRAY);
}

void CPlayer::ShowProgressBar() const
{
	int pos;
	int progress_bar_start{ 5 };
	int progress_bar_length{ m_width - progress_bar_start - 13 };
	wstring progress_bar(progress_bar_length, '-');		//生成用于表示进度条的由'-'组成的字符串
	//GetCurrentPosition();
	//GetSongLength();
	if (m_song_length_int > 0)
		pos = m_current_position_int * progress_bar_length / m_song_length_int;
	else
		pos = 0;
	if (pos >= progress_bar_length) pos = progress_bar_length - 1;
	progress_bar[pos] = '*';		//将进度条当前位置的字符替换成*
	PrintWString(progress_bar.c_str(), progress_bar_start, 2, pos + 1, PURPLE, DARK_PURPLE);

	//显示>>>>
	int i;
	if (!FileIsMidi(m_current_file_name))
		i = m_current_position.sec % 4 + 1;
	else
		i = m_current_position_int / 4 % 4 + 1;

	if (m_playing == 0) i = 0;
	PrintWString(L">>>>", 0, 2, i, GREEN, GRAY);

	//显示歌曲时间
	wchar_t buff[15];
	if (!FileIsMidi(m_current_file_name))		//播放的不是MIDI音乐时显示时间的分钟数和秒数
		swprintf_s(buff, sizeof(buff)/2, L"%d:%.2d/%d:%.2d", m_current_position.min, m_current_position.sec, m_song_length.min, m_song_length.sec);
	else		//播放MIDI音乐时显示位置
		swprintf_s(buff, sizeof(buff)/2, L"%d/%d", m_current_position_int, m_song_length_int);
	ClearString(m_width - 12, 2, 12);
	PrintWString(buff, m_width - 12, 2, DARK_YELLOW);
}

void CPlayer::ShowPlaylist() const
{
	int x, y{ 4 }, playlist_width;
	x = m_width < WIDTH_THRESHOLD ? 0 : (m_width / 2 + 1);
	playlist_width = m_width < WIDTH_THRESHOLD ? m_width : (m_width / 2 - 1);
	wchar_t buff[16];
	PrintWString(L"播放列表", x, y, GREEN);
	swprintf_s(buff, sizeof(buff)/2, L"(共%d首)：", m_song_num);
	PrintWString(buff, x + 8, y, GREEN);
	PrintWString(L"当前路径：", x, y + 1, GRAY);
	PrintWString(m_path.c_str(), x + 10, y + 1, playlist_width - 10, GRAY);		//路径最多显示playlist_width - 10个字符
	for (int i{ 0 }; i < m_song_per_page; i++)
		ClearString(x, y + 2 + i, 2);

	if (m_song_num > 0 && m_display_page == m_index / m_song_per_page + 1)		//如果显示的是正在播放歌曲所在页，就显示当前歌曲指示
		PrintWString(L"◆", x, y + 2 + m_index%m_song_per_page, CYAN);

	for (int i{ 0 }; i < m_song_per_page; i++)
	{
		int index{ i + m_song_per_page *(m_display_page - 1) };
		if (index < m_song_num)
		{
			PrintInt(index + 1, x + 2, y + 2 + i, GRAY);		//输出序号
			PrintWString(m_playlist[index].c_str(), x + 6, y + 2 + i, playlist_width - 13, WHITE);		//输出文件名（最多只输出playlist_width - 13个字符）
			if (!FileIsMidi(m_playlist[index]))
				swprintf_s(buff, sizeof(buff)/2, L"%d:%.2d", m_all_song_length[index].min, m_all_song_length[index].sec);
			else
				swprintf_s(buff, sizeof(buff)/2, L"%d", time2int(m_all_song_length[index]));		//midi音乐的长度只能用int型表示
			if (m_all_song_length[index] > Time{0, 0, 0})
				PrintWString(buff, m_width - 6, y + 2 + i, DARK_YELLOW);		//输出音频文件长度
			else
				PrintWString(L"-:--", m_width - 6, y + 2 + i, DARK_YELLOW);		//获取不到长度时显示-:--
		}
	}
	//PrintWString(L"按[]翻页", m_width - 14, m_hight - 1, DARK_CYAN);
	swprintf_s(buff, sizeof(buff)/2, L"按[]翻页 %d/%d", m_display_page, m_total_page);
	PrintWString(buff, m_total_page > 99 ? m_width - 16 : m_width - 14, m_hight - 1, DARK_CYAN);

	if (!m_Lyrics.IsEmpty())
	{
		ClearString(m_width - 36, m_hight - 1, 19);
		PrintWString(L"歌词编码：", m_width - 36, m_hight - 1, GRAY);
		switch (m_Lyrics.GetCodeType())
		{
		case CodeType::ANSI: PrintWString(L"ANSI", m_width - 26, m_hight - 1, GRAY); break;
		case CodeType::UTF8: PrintWString(L"UTF8", m_width - 26, m_hight - 1, GRAY); break;
		case CodeType::UTF8_NO_BOM: PrintWString(L"UTF8无BOM", m_width - 26, m_hight - 1, GRAY); break;
		default: break;
		}
	}
}

void CPlayer::ShowLyrics(bool force_refresh) const
{
	if (m_width < WIDTH_THRESHOLD)
		ShowLyricsSingleLine();
	else
		ShowLyricsMultiLine(force_refresh);
}

void CPlayer::ShowLyricsSingleLine() const
{
	if (!m_Lyrics.IsEmpty())
	{
		static wstring last_lyric;
		wstring current_lyric{ m_Lyrics.GetLyric(m_current_position, 0) };		//获取当前歌词
		if (current_lyric != last_lyric)
		{
			last_lyric = current_lyric;
			ClearString(0, 3, m_width);				//只有当前歌词变了才要清除歌词显示区域
		}
		wstring temp;
		int lyric_progress{ static_cast<int>(m_Lyrics.GetLyricProgress(m_current_position)*(WcharStrHalfWidthLen(current_lyric.c_str()) + 1) / 1000) };
		/*lyric_progress为当前歌词的进度所在的字符数，根据GetLyricProgress函数获得的歌词进度和当前歌词的长度计算得到，
		用于以卡拉OK形式显示歌词*/
		int lrc_start, lrc_start_half_width;		//当前歌词从第几个字符开始显示（用于当歌词文本超过指定长度时）
		int x{ static_cast<int>(m_width/2 - WcharStrHalfWidthLen(current_lyric.c_str())/2) };		//x为输出歌词文本的x位置（用于实现歌词居中显示）
		if (x < 0) x = 0;
		if (WcharStrHalfWidthLen(current_lyric.c_str()) <= m_width)
		{
			PrintWString(current_lyric.c_str(), x, 3, lyric_progress, CYAN, DARK_CYAN);
		}
		else				//歌词文本超过指定的长度时滚动显示
		{
			current_lyric.append(L" ");		//在歌词末尾加上一个空格，用于解决有时歌词最后一个字符无法显示的问题
			if (lyric_progress < m_width/2)		//当前歌词进度小于控制台宽度一半时，当前歌词从第0个字符开始显示
			{
				lrc_start = 0;
				lrc_start_half_width = 0;
			}
			else		//当前歌词进度大于等于控制台宽度一半时，需要计算当前歌词从第几个字符开始显示
			{
				lrc_start = lyric_progress - m_width / 2;
				if (lrc_start > WcharStrHalfWidthLen(current_lyric.c_str()) - m_width) lrc_start = WcharStrHalfWidthLen(current_lyric.c_str()) - m_width;
				lrc_start_half_width = lrc_start;
				//此时得到的lrc_start是以半角字符为单位，下面一行代码用于计算实际lrc_start的值
				//用lrc_start减去lrc_start位置前面的全角字符数
				lrc_start = lrc_start - FullWidthCount(current_lyric.c_str(), lrc_start);
				if (lrc_start < 0) lrc_start = 0;
			}
			temp = current_lyric.substr(lrc_start, m_width);
			PrintWString(temp.c_str(), x, 3, m_width, lyric_progress - lrc_start_half_width, CYAN, DARK_CYAN);
		}
	}
	else
		PrintWString(L"当前歌曲没有歌词", m_width / 2 - 8, 3, DARK_CYAN);
}

void CPlayer::ShowLyricsMultiLine(bool force_refresh) const
{
	int x{ 0 }, y{ 4 };			//歌词界面的起始x和y坐标
	int lyric_width{ m_width / 2 - 1 };		//歌词显示的宽度
	int lyric_hight{ m_hight - 6 };		//歌词显示的高度
	int lyric_x;		//每一句歌词输出时的起始x坐标
	wstring lyric_text;		//储存每一句歌词文本
	static bool lyric_change_flag{ true };
	if(lyric_change_flag || force_refresh) PrintWString(L"歌词秀：", x, y, GREEN);
	if (!m_Lyrics.IsEmpty())
	{
		static int last_lyric_index;
		int current_lyric_index{ m_Lyrics.GetLyricIndex(m_current_position) };		//获取当前歌词编号
		if (current_lyric_index != last_lyric_index)				//如果当前歌词变了，就将歌词区域全部清除
		{
			lyric_change_flag = true;
			last_lyric_index = current_lyric_index;
			for (int i = 0; i < lyric_hight; i++)
				ClearString(x, i + y + 1, lyric_width);
		}
		else
		{
			lyric_change_flag = false;
		}

		//for (int i{ -lyric_hight / 2 + 1 }; i <= lyric_hight / 2; i++)
		for (int i{ -lyric_hight / 2 + 1 }; i + lyric_hight / 2 <= lyric_hight; i++)
		{
			lyric_text = m_Lyrics.GetLyric(m_current_position, i);
			if (lyric_text == L"……") lyric_text.clear();		//多行模式下不显示省略号
			lyric_x = lyric_width / 2 - WcharStrHalfWidthLen(lyric_text.c_str()) / 2;
			if (lyric_x < 0) lyric_x = 0;
			if (i != 0)		//不是当前歌词，以暗色显示
			{
				if (lyric_change_flag || force_refresh)			//不是当前歌词只有当前歌词变化了或参数要求强制刷新时才刷新，以避免反复刷新歌词，减少闪烁
					PrintWString(lyric_text.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_width, DARK_CYAN);
			}
			else		//i=0时为当前歌词
			{
				int lyric_progress{ static_cast<int>(m_Lyrics.GetLyricProgress(m_current_position)*(WcharStrHalfWidthLen(lyric_text.c_str()) + 1) / 1000) };		//歌词进度
				int lrc_start, lrc_start_half_width;
				if (WcharStrHalfWidthLen(lyric_text.c_str()) <= lyric_width)		//当前歌词宽度小于歌词显示宽度时直接显示
				{
					PrintWString(lyric_text.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_progress, CYAN, DARK_CYAN);
				}
				else		//当前歌词宽度大于歌词显示宽度时滚动显示
				{
					lyric_text.append(L" ");		//在歌词末尾加上一个空格，用于解决有时歌词最后一个字符无法显示的问题
					wstring temp;
					if (lyric_progress < lyric_width / 2)		//当前歌词进度小于歌词显示宽度一半时，当前歌词从第0个字符开始显示
					{
						lrc_start = 0;
						lrc_start_half_width = 0;
					}
					else		//当前歌词进度大于等于控制台宽度一半时，需要计算当前歌词从第几个字符开始显示
					{
						lrc_start = lyric_progress - lyric_width / 2;
						if (lrc_start > WcharStrHalfWidthLen(lyric_text.c_str()) - lyric_width) lrc_start = WcharStrHalfWidthLen(lyric_text.c_str()) - lyric_width;
						lrc_start_half_width = lrc_start;
						//此时得到的lrc_start是以半角字符为单位，下面一行代码用于计算实际lrc_start的值
						//用lrc_start减去lrc_start位置前面的全角字符数
						lrc_start = lrc_start - FullWidthCount(lyric_text.c_str(), lrc_start);
					}
					if (lrc_start < 0) lrc_start = 0;
					temp = lyric_text.substr(lrc_start, lyric_width);
					PrintWString(temp.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_width, lyric_progress - lrc_start_half_width, CYAN, DARK_CYAN);
				}
			}
		}
	}
	else
		PrintWString(L"当前歌曲没有歌词", lyric_width / 2 - 8, y + lyric_hight / 2, DARK_CYAN);
}


void CPlayer::SwitchPlaylist(int operation)
{
	if (operation == NEXT)
	{
		m_display_page++;
		if (m_display_page > m_total_page)
			m_display_page = 1;
	}
	if (operation == PREVIOUS)
	{
		m_display_page--;
		if (m_display_page < 1)
			m_display_page = m_total_page;
	}
	system("cls");
	// ShowInfo();
	// ShowProgressBar();
	// ShowLyrics();
	// ShowPlaylist();
}

int CPlayer::GetCurrentSecond()
{
	//GetCurrentPosition();
	if (!FileIsMidi(m_current_file_name))
		return m_current_position.sec;		//不是midi音乐时每1秒钟刷新
	else return m_current_position_int / 4;		//midi音乐时每4个位置刷新
}

inline bool CPlayer::IsPlaying() const
{
	return m_playing == 2;
}

bool CPlayer::PlayTrack(int song_track)
{
	switch (m_repeat_mode)
	{
	case 0:		//顺序播放
		if (song_track == NEXT)		//播放下一曲
			song_track = m_index + 1;
		if (song_track == PREVIOUS)		//播放上一曲
			song_track = m_index - 1;
		break;
	case 1:		//随机播放
		if (song_track == NEXT || song_track == PREVIOUS)
		{
			SYSTEMTIME current_time;
			GetLocalTime(&current_time);			//获取当前时间
			srand(current_time.wMilliseconds);		//用当前时间的毫秒数设置产生随机数的种子
			song_track = rand() % m_song_num;
		}
		break;
	case 2:		//列表循环
		if (song_track == NEXT)		//播放下一曲
		{
			song_track = m_index + 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		if (song_track == PREVIOUS)		//播放上一曲
		{
			song_track = m_index - 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		break;
	case 3:		//单曲循环
		if (song_track == NEXT || song_track == PREVIOUS)
			song_track = m_index;
	}

	if (song_track >= 0 && song_track < m_song_num)
	{
		MusicControl(Command::CLOSE);
		m_index = song_track;
		m_display_page = m_index / m_song_per_page + 1;		//切歌时播放列表自动跳转到播放歌曲所在页
		m_current_file_name = m_playlist[m_index];
		MusicControl(Command::OPEN);
		SetVolume();
		IniLyrics();
		MusicControl(Command::PLAY);
		GetCurrentPosition();
		system("cls");
		ShowInfo();
		ShowProgressBar();
		ShowLyrics(true);
		ShowPlaylist();
		SetTitle();
		SaveConfig();
		return true;
	}
	return false;
}

void CPlayer::ChangePath(const wstring& path, int track)
{
	MusicControl(Command::CLOSE);
	m_path = path;
	if (m_path.empty() || (m_path.back() != L'/' && m_path.back() != L'\\'))		//如果输入的新路径为空或末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'\\');
	m_playlist.clear();		//清空播放列表
	m_index = track;
	//初始化播放列表
	IniPlayList();		//根据新路径重新初始化播放列表
	m_current_position_int = 0;
	m_current_position = { 0, 0, 0 };
	SaveConfig();
	SetTitle();
	system("cls");
	ShowInfo();
	ShowProgressBar();
	ShowPlaylist();
	MusicControl(Command::OPEN);
	SetVolume();
	IniLyrics();
	ShowLyrics(true);
	m_find_result.clear();	//更改路径后清空查找结果
}

void CPlayer::SetPath()
{
	//string path;
	//PrintWString(L"修改当前路径将清空当前播放列表，是否继续？是(Y)|刷新(R)|取消(N)", 0, 2, RED);
	//switch (GetKey())
	//{
	//case 'Y':		//输入Y之后重新从键盘输入路径，并重新初始化播放列表
	//	ShowProgressBar();
	//	//MusicControl(Command::CLOSE);
	//	CursorVisible(true);
	//	do
	//	{
	//		GotoXY(m_width < WIDTH_THRESHOLD ? 10 : (m_width / 2 + 11), 5);
	//		std::getline(std::cin, path);		//从键盘输入路径
	//	}while (path.empty());
	//	CursorVisible(false);
	//	ChangePath(path);
	//	break;
	//case 'R':		//输入R时重新初始化列表
	//	ChangePath(m_path);
	//	break;
	//default:
	//	ShowPlaylist();
	//	break;
	//}
	const int x{ 0 }, y{ 4 };
	int hight{ m_hight - 5 };
	wstring path;
	int item_select{ 0 };
	int max_selection;
	wchar_t buff[256];
	for (int i{ 0 }; i < hight; i++)
		ClearString(0, 4 + i, m_width);

	PrintWString(L"设置路径", x, y, CYAN);
	PrintWString(L"当前路径：", x, y + 1, GRAY);
	PrintWString(m_path.c_str(), x + 10, y + 1, m_width - 10, GRAY);		//路径最多显示m_width - 10个字符
	PrintWString(L"帮助：方向键选择，回车键确定，D键删除选中的路径，ESC键返回。", x, m_hight - 2, GRAY);
	ClearString(m_width - 16, m_hight - 1, 16);		//清除右下角的“按[]翻页”

	while (true)
	{
		swprintf_s(buff, sizeof(buff) / 2, L"共%d个", m_recent_path.size());
		PrintWString(buff, 9, y, DARK_WHITE);
		max_selection = m_recent_path.size() + 1;
		if (max_selection > hight - 3) max_selection = hight - 3;
		for (int i{ 0 }; i < max_selection; i++)
		{
			if (i == item_select)
				PrintWString(L"◆", x, y + 2 + i, CYAN);
			else
				ClearString(x, y + 2 + i, 2);
			
			if (i == 0)
			{
				PrintWString(L"输入新路径", x + 2, y + 2 + i, YELLOW);
			}
			else
			{
				PrintInt(i, x + 2, y + 2 + i, DARK_YELLOW);		//输出序号
				swprintf_s(buff, sizeof(buff)/2, L"%s (播放到第%d首)", std::get<PATH>(m_recent_path[i - 1]).c_str(), std::get<TRACK>(m_recent_path[i - 1]) + 1);
				size_t path_length{ WcharStrHalfWidthLen(std::get<PATH>(m_recent_path[i - 1]).c_str()) };	//路径字符串占的半角字符数
				PrintWString(buff, x + 5, y + 2 + i, m_width - 5, path_length, YELLOW, GRAY);
			}
		}

		switch (GetKey())
		{
		case UP_KEY:
			item_select--;
			if (item_select < 0) item_select = max_selection - 1;
			break;
		case DOWN_KEY:
			item_select++;
			if (item_select >= max_selection) item_select = 0;
			break;
		case 'D':		//按下D键，删除选中路径
			if (item_select > 0 && m_recent_path.size() > 0)	//第0项为输入一个路径，不能执行此操作；同时删除时确保m_recent_path的元素个数大于0
			{
				m_recent_path.erase(m_recent_path.begin() + item_select - 1);
				for (int i{ item_select }; i <= max_selection; i++)
					ClearString(x, y + i + 1, m_width);
				SaveRecentPath();
				max_selection--;
				if (item_select >= max_selection) item_select = max_selection - 1;
			}
			break;
		case ENTER_KEY:
			if (m_song_num>0) EmplaceCurrentPathToRecent();		//如果当前路径有歌曲，就保存当前路径到最近路径
			SaveRecentPath();
			switch (item_select)
			{
			case 0:		//选择第0项，输入一个新路径
				CursorVisible(true);
				do
				{
					GotoXY(10, 5);
					std::getline(std::wcin, path);		//从键盘输入路径
				}while (path.empty());
				CursorVisible(false);
				if (path.size() > 1)		//路径必须至少2个字符，如果只输入1个字符就忽略它
				{
					ChangePath(path);
					EmplaceCurrentPathToRecent();		//保存新的路径到最近路径
				}
				break;
			default:
				//m_index = std::get<TRACK>(m_recent_path[item_select - 1]);
				ChangePath(std::get<PATH>(m_recent_path[item_select - 1]), std::get<TRACK>(m_recent_path[item_select - 1]));		//设置当前路径为选中的路径
				m_current_position_int = std::get<POSITION>(m_recent_path[item_select - 1]);
				m_current_position = int2time(m_current_position_int);
				MusicControl(Command::SEEK);
				EmplaceCurrentPathToRecent();		//将选中的路径移到第1位
				break;
			}
			SaveRecentPath();
			//按下回车键直接返回，这里没有break。
		case ESC_KEY:
			system("cls");
			ShowInfo();
			ShowProgressBar();
			ShowLyrics();
			ShowPlaylist();
			return;
		default:
			break;
		}
	}
}

void CPlayer::SetTrack()
{
	int track;
	PrintWString(L"请输入要播放的歌曲序号（输入0取消）：", 0, 2, DARK_WHITE);
	GotoXY(37, 2);
	CursorVisible(true);
	std::cin >> track;
	CursorVisible(false);
	PlayTrack(track - 1);
}

void CPlayer::SetRepeatMode()
{
	m_repeat_mode++;
	if (m_repeat_mode > 3)
		m_repeat_mode = 0;
	ShowInfo();
	SaveConfig();
}

bool CPlayer::ErrorDispose()
{
	if (m_song_num==0)
	{
		PrintWString(L"当前路径下没有音频文件，请按任意键重新设置文件路径。", 0, 2, DARK_WHITE);
		_getch();
		if (!m_recent_path.empty() && m_path == std::get<PATH>(m_recent_path[0]))
			m_recent_path.erase(m_recent_path.begin());		//当前路径没有音频文件，把最近路径中的没有文件的路径删除
		SetPath();
		if (m_error_code) return false;
		else return true;
	}
	
	wchar_t error_info[64];
	if (m_error_code)
	{
		mciGetErrorStringW(m_error_code, error_info, sizeof(error_info) / 2);		//根据错误代码获取错误信息，并储存在error_info字符数组中
		PrintWString(L"错误：", 0, 2, DARK_WHITE);
		PrintWString(error_info, 6, 2, DARK_WHITE);		//输出错误信息
		system("pause");
		switch (m_error_code)
		{
		case 263:				//出现“指定的设备未打开，或不被 MCI 所识别。”错误，可能是路径不正确
			PrintWString(L"出现错误，是否重新设置路径？是(Y)|否(N)", 0, 2, RED);
			if (GetKey() == 'Y')
				SetPath();				//重新设置路径
			break;
		default:
			break;
		}
		// if (m_error_code) return false;
		// else return true;
	}
	return true;
}

inline void CPlayer::SetTitle() const
{
	SetConsoleTitleW((m_current_file_name + L" - MusicPlayer").c_str());		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::SaveConfig() const
{
	WritePrivateProfileStringW(L"config",L"path", m_path.c_str(), m_config_path.c_str());
	WritePrivateProfileIntW(L"config",L"track", m_index, m_config_path.c_str());
	WritePrivateProfileIntW(L"config", L"volume", m_volume, m_config_path.c_str());
	WritePrivateProfileIntW(L"config", L"position", m_current_position_int, m_config_path.c_str());
	WritePrivateProfileIntW(L"config", L"repeat_mode", m_repeat_mode, m_config_path.c_str());
	WritePrivateProfileIntW(L"config", L"window_width", m_width, m_config_path.c_str());
	WritePrivateProfileIntW(L"config", L"window_hight", m_hight, m_config_path.c_str());
	//字体和字体大小设置只需在程序启动从ini文件读取，FullWidthCount
	//WritePrivateProfileStringW(L"config",L"font", m_font, INI_PATH_W);
	//WritePrivateProfileInt("config", "font_size", m_font_size, INI_PATH);
}

void CPlayer::LoadConfig()
{
	wchar_t buff[256];
	GetPrivateProfileStringW(L"config",L"path", L".\\songs\\", buff, 255, m_config_path.c_str());
	m_path = buff;
	if(m_path.back() != L'/' && m_path.back() !=L'\\')		//如果读取到的新路径末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'\\');
	m_index = GetPrivateProfileIntW(L"config",L"track", 0, m_config_path.c_str());
	m_volume = GetPrivateProfileIntW(L"config", L"volume", 100, m_config_path.c_str());
	m_current_position_int = GetPrivateProfileIntW(L"config", L"position", 0, m_config_path.c_str());
	m_current_position = int2time(m_current_position_int);
	m_repeat_mode = GetPrivateProfileIntW(L"config", L"repeat_mode", 0, m_config_path.c_str());
	m_width = GetPrivateProfileIntW(L"config", L"window_width", 80, m_config_path.c_str());
	m_hight = GetPrivateProfileIntW(L"config", L"window_hight", 25, m_config_path.c_str());
	GetPrivateProfileStringW(L"config", L"font", L"新宋体", m_font, sizeof(m_font) / sizeof(wchar_t), m_config_path.c_str());
	m_font_size = GetPrivateProfileIntW(L"config", L"font_size", 14, m_config_path.c_str());
	GetPrivateProfileStringW(L"config", L"lyric_path", L".\\lyrics\\", buff, 255, m_config_path.c_str());
	m_lyric_path = buff;
	if (m_lyric_path.back() != L'/' && m_lyric_path.back() != L'\\')
		m_lyric_path.append(1, L'\\');
}

void CPlayer::GetWindowsSize(int width, int hight)
{
	m_width = width;
	m_hight = hight;
	m_song_per_page = m_hight - 7;
	//重新计算所有需要根据窗口宽度和高度变化而变化的数据
	if (m_song_num == 0)
		m_total_page = 1;
	else
		m_total_page = (m_song_num - 1) / m_song_per_page + 1;
	m_display_page = m_index / m_song_per_page + 1;		//设定播放列表中当前显示的页
}

inline void CPlayer::ExplorePath() const
{
	system(("explorer.exe \"" + UnicodeToStr(m_path, CodeType::ANSI) + '\"').c_str());
}

void CPlayer::Find()
{
	const int x{ 0 }, y{ 4 };
	int hight{ m_hight - 5 };
	int item_select{ 0 };
	int max_selection{ static_cast<int>(m_find_result.size()) };
	wchar_t buff[256];
	wstring key_word;
	bool find_flag{ false };	//执行过一次查找后，find_flag会被置为true
	for (int i{ 0 }; i < hight; i++)
		ClearString(0, 4 + i, m_width);

	PrintWString(L"查找文件", x, y, CYAN);
	PrintWString(L"请按空格键后输入要查找的关键词：", x, y + 1, DARK_WHITE);
	while (true)
	{
		max_selection = m_find_result.size();
		if (max_selection > hight - 4) max_selection = hight - 4;
		//清除显示查找结果区域的字符
		for (int i{ 0 }; i < m_hight - 8; i++)
			ClearString(x, y + 2 + i, m_width);

		if (!m_find_result.empty())
		{
			if (find_flag)
			{
				PrintWString(L"查找结果：", x, y + 2, GREEN);
				swprintf_s(buff, sizeof(buff) / 2, L"(共%d个结果)", m_find_result.size());
				PrintWString(buff, x + 10, y + 2, GRAY);
			}
			else
			{
				PrintWString(L"上次的查找结果：", x, y + 2, GREEN);
				swprintf_s(buff, sizeof(buff) / 2, L"(共%d个结果)", m_find_result.size());
				PrintWString(buff, x + 16, y + 2, GRAY);
			}
			for (int i{ 0 }; i < max_selection; i++)
			{
				if (i == item_select)
					PrintWString(L"◆", x, y + 3 + i, CYAN);
				else
					ClearString(x, y + 3 + i, 2);

				//查找结果显示格式为：序号 文件名 (分/秒)
				PrintInt(i + 1, x + 2, y + 3 + i, GRAY);
				swprintf_s(buff, sizeof(buff) / 2, L"%s (%d:%.2d)", m_playlist[m_find_result[i]].c_str(), m_all_song_length[m_find_result[i]].min, m_all_song_length[m_find_result[i]].sec);
				PrintWString(buff, x + 5, y + 3 + i, m_width - 5, WcharStrHalfWidthLen(m_playlist[m_find_result[i]].c_str()), WHITE, DARK_YELLOW);
			}
		}
		else if(find_flag)
		{
			PrintWString(L"没有找到结果", x, y + 2, YELLOW);
		}

		PrintWString(L"帮助：方向键选择查找结果，回车键播放，空格键开始查找，输入“*”取消查找。ESC键返回。", x, m_hight - 2, GRAY);
		ClearString(m_width - 16, m_hight - 1, 16);
		switch (GetKey())
		{
		case SPACE_KEY:
			ClearString(x + 32, y + 1, m_width - x - 32);	//输入前清除上次输入的字符
			CursorVisible(true);
			do
			{
				GotoXY(x + 32, y + 1);
				std::getline(std::wcin, key_word);		//从键盘输入关键词
			} while (key_word.empty());
			CursorVisible(false);
			if (key_word == L"*")		//输入星号取消查找
				break;
			find_flag = true;
			m_find_result.clear();		//查找之前先清除上一次的查找结果
			FindFile(key_word);
			break;
		case UP_KEY:
			item_select--;
			if (item_select < 0) item_select = max_selection - 1;
			break;
		case DOWN_KEY:
			item_select++;
			if (item_select >= max_selection) item_select = 0;
			break;
		case ENTER_KEY:
			if(!m_find_result.empty())
				PlayTrack(m_find_result[item_select]);
		case ESC_KEY:
			system("cls");
			ShowInfo();
			ShowProgressBar();
			ShowLyrics();
			ShowPlaylist();
			return;
		default:
			break;
		}
	}
}

void CPlayer::FindFile(const wstring & key_word)
{
	int index;
	for (int i{ 0 }; i < m_playlist.size(); i++)
	{
		index = m_playlist[i].find(key_word);
		if (index != string::npos)
			m_find_result.push_back(i);
	}
}


//void CPlayer::GetConsoleSetting()
//{
//	CONSOLE_FONT_INFOEX info;
//	GetCurrentConsoleFontEx(handle, NULL, &info);
//	m_font_size = info.FontWeight;
//	wcscpy_s(m_font, info.FaceName);
//}

void CPlayer::SaveRecentPath() const
{
	//将最近文件列表保存到程序目录下的recent_path.dat中，其中每一行的格式为：路径<曲目序号,播放到的位置(毫秒)>
	ofstream SaveFile{ m_recent_path_dat_path };
	wchar_t buff_unicode[256];
	for (auto& path_info : m_recent_path)
	{
		swprintf_s(buff_unicode, sizeof(buff_unicode) / 2, L"%s<%.4d,%d>\n", std::get<PATH>(path_info).c_str(), std::get<TRACK>(path_info), std::get<POSITION>(path_info));
		SaveFile << UnicodeToStr(wstring{ buff_unicode }, CodeType::UTF8);		//以UTF8格式保存路径
	}
	SaveFile.close();
}

void CPlayer::LoadRecentPath()
{
	ifstream OpenFile{ m_recent_path_dat_path };
	if (!FileExist(m_recent_path_dat_path)) return;
	while (!OpenFile.eof())
	{
		string current_line, temp;
		wstring path;
		int track, position;
		wchar_t buff_unicode[256]{ 0 };
		int index;
		std::getline(OpenFile, current_line);		//读取文件中的每一行
		if (current_line.size() >= 3 && (current_line[0] == -17 && current_line[1] == -69 && current_line[2] == -65))
			current_line = current_line.substr(3);		//如果RecentPath文件的前面3个字节是UTF8文件的BOM，就把它们删除
		index = current_line.find_first_of('<');
		if (index == string::npos)	//没有找到'<'，说明文件中只保存了路径而没有保存曲目序号和位置
		{
			path = StrToUnicode(current_line, CodeType::UTF8);
			track = 0;
			position = 0;
		}
		else
		{
			path = StrToUnicode(current_line.substr(0, index), CodeType::UTF8);		//读取路径
			temp = current_line.substr(index + 1, 4);	//读取曲目序号（固定4个字符）
			track = atoi(temp.c_str());
			temp = current_line.substr(index + 6, current_line.size() - index - 5);		//读取位置
			position = atoi(temp.c_str());
		}
		if (path.empty() || path.size() < 2) continue;		//如果路径为空或路径太短，就忽略它
		if (path.back() != L'/' && path.back() != L'\\')	//如果读取到的路径末尾没有斜杠，则在末尾加上一个
			path.append(1, L'\\');
		m_recent_path.push_back(std::make_tuple(path, track, position));
	}
	OpenFile.close();
}

void CPlayer::EmplaceCurrentPathToRecent()
{
	for (int i{ 0 }; i < m_recent_path.size(); i++)
	{
		if (m_path == std::get<PATH>(m_recent_path[i]))
			m_recent_path.erase(m_recent_path.begin() + i);		//如果当前路径已经在最近路径中，就把它最近路径中删除
	}
	m_recent_path.emplace_front(std::make_tuple(m_path, m_index, m_current_position_int));		//当前路径插入到m_recent_path的前面
}
