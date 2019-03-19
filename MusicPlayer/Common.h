//************************************************************
//		本文件为全局变量、宏、枚举类型和全局函数的定义
//************************************************************
#pragma once
#include<conio.h >
#include <vector>
#include <io.h>
#include<string>
#include<Windows.h>
using std::string;
using std::wstring;
using std::vector;

#define UP_KEY (-1)			//定义上方向键
#define DOWN_KEY (-2)		//定义下方向键
#define LEFT_KEY (-3)		//定义左方向键
#define RIGHT_KEY (-4)		//定义右方向键
#define ESC_KEY 27
#define SPACE_KEY ' '
#define ENTER_KEY 13
#define NEXT (-999)		//定义“下一曲”常量
#define PREVIOUS (-998)		//定义“上一曲”常量

//#define PROGRESS_BAR_LEN 67		//定义进度条占用的字符数
//#define SONG_NAME_LEN 50		//定义歌曲标题显示的字符数
//#define LYRIC_LEN 80		//定义歌词显示的字符数
#define LYRIC_REFRESH 100		//定义歌词刷新的间隔时间（毫秒）

#define MIN_WIDTH 40		//定义控制台窗口的最小宽度
#define MIN_HIGHT 10		//定义控制台窗口的最小高度

#define WIDTH_THRESHOLD 80		//定义歌词从单行显示切换到多行显示时的界面宽度阈值

#define MAX_NUM_LENGTH 80		//定义获取音频文件长度数量的最大值

const wchar_t* VERSION = L"1.35";	//程序版本
//const char* INI_PATH = ".\\config.ini";		//配置文件路径
//const wchar_t* INI_PATH_W = L".\\config.ini";	//配置文件路径（宽字符）
//const char* RECENT_FILE_PATH = ".\\recent_path.dat";	//最近播放路径文件的路径
//const wchar_t* RECENT_FILE_PATH_W = L".\\recent_path.dat";	//最近播放路径文件的路径（宽字符）

enum class Command
{
	OPEN,
	PLAY,
	CLOSE,
	PAUSE,
	STOP,
	PLAY_PAUSE,
	FF,	//快进
	REW,		//快倒
	VOLUME_UP,
	VOLUME_DOWN,
	SEEK
};

enum Color
{
	BLACK = 0,
	DARK_BLUE,
	DARK_GREEN,
	DARK_CYAN,
	DARK_RED,
	DARK_PURPLE,
	DARK_YELLOW,
	DARK_WHITE,
	GRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	YELLOW,
	WHITE
};

enum class CodeType
{
	ANSI,
	UTF8,
	UTF8_NO_BOM
};

struct Time
{
	int min;
	int sec;
	int msec;
};

//将int类型的时间（毫秒数）转换成Time结构
Time int2time(int time)
{
	Time result;
	result.msec = time % 1000;
	result.sec = time / 1000 % 60;
	result.min = time / 1000 / 60;
	return result;
}

//将Time结构转换成int类型（毫秒数）
inline int time2int(Time time)
{
	return time.msec + time.sec * 1000 + time.min * 60000;
}

bool operator>(Time time1, Time time2)
{
	if (time1.min != time2.min)
		return (time1.min > time2.min);
	else if (time1.sec != time2.sec)
		return(time1.sec > time2.sec);
	else if (time1.msec != time2.msec)
		return(time1.msec > time2.msec);
	else return false;
}

bool operator==(Time time1, Time time2)
{
	return (time1.min == time2.min && time1.sec == time2.sec && time1.msec == time2.msec);
}

bool operator>=(Time time1, Time time2)
{
	if (time1.min != time2.min)
		return (time1.min > time2.min);
	else if (time1.sec != time2.sec)
		return(time1.sec > time2.sec);
	else if (time1.msec != time2.msec)
		return(time1.msec > time2.msec);
	else return true;
}

int operator-(Time time1, Time time2)
{
	return (time1.min - time2.min) * 60000 + (time1.sec - time2.sec) * 1000 + (time1.msec - time2.msec);
}

//为SYSTEMTIME结构重载减号运算符，在确保a>b的情况下返回两个时间差的毫秒数，但是返回的值不会超过3000毫秒
int operator-(SYSTEMTIME a, SYSTEMTIME b)
{
	if (a.wSecond == b.wSecond)
		return a.wMilliseconds - b.wMilliseconds;
	else if (a.wSecond - b.wSecond == 1 || a.wSecond - b.wSecond == -59)
		return a.wMilliseconds - b.wMilliseconds + 1000;
	else
		return a.wMilliseconds - b.wMilliseconds + 2000;
}


//获取一个键盘输入按键并将其返回
int GetKey()
{
	int key{ _getch() };
	if (key == 0xE0 || key == 0)		//如果获得的按键值为0x0E或0则表示按下了功能键
	{
		switch (_getch())		//按下了功能键需要再次调用_getch函数
		{
			case 72: key = UP_KEY; break;
			case 80: key = DOWN_KEY; break;
			case 75: key = LEFT_KEY; break;
			case 77: key = RIGHT_KEY; break;
			default: break;
		}
	}
	if (key >= 'a' && key <= 'z')		//如果按的是小写字母，则自动转换成大写字母
		key -= 32;
	return key;
}

//获取path路径下的指定格式的文件的文件名，并保存在files容器中。format容器中储存着要获取的文件格式。最多只获取max_files个文件。
void GetAllFormatFiles(wstring path, vector<wstring>& files, const vector<wstring>& format, size_t max_file = 99999)
{
	//文件句柄 
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	wstring file_path;
	for (auto a_format : format)
	{
		if ((hFile = _wfindfirst(file_path.assign(path).append(L"\\*.").append(a_format).c_str(), &fileinfo)) != -1)
		{
			do
			{
				if (files.size() >= max_file) break;
				files.push_back(file_path.assign(fileinfo.name));  //将文件名保存
			} while (_wfindnext(hFile, &fileinfo) == 0);
		}
		_findclose(hFile);
	}
}

bool FileExist(const wstring& file)
{
	int hFile = 0;
	_wfinddata_t fileinfo;
	return ((hFile = _wfindfirst(file.c_str(), &fileinfo)) != -1);
}

//判断文件类型是否为midi音乐
bool FileIsMidi(const wstring& file_name)
{
	size_t length{ file_name.size() };
	return (file_name.substr(length - 4, 4) == L".mid" || file_name.substr(length - 5, 5) == L".midi");
}

//向ini文件写入一个int数据
inline void WritePrivateProfileIntW(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path)
{
	wchar_t buff[11];
	_itow_s(value, buff, 10);
	WritePrivateProfileStringW(AppName, KeyName, buff, Path);
}

//将一个UTF8编码的字符串转换成ANSI编码
//int Utf8ToAnsi(const char *pstrUTF8, char *pstrAnsi)
//{
//	int i = 0;
//	int j = 0;
//	wchar_t strUnicode[200] = { 0 };
//	i = MultiByteToWideChar(CP_UTF8, 0, pstrUTF8, -1, NULL, 0);
//	memset(strUnicode, 0, i);
//	MultiByteToWideChar(CP_UTF8, 0, pstrUTF8, -1, strUnicode, i);
//	j = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
//	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pstrAnsi, j, NULL, NULL);
//	return 0;
//}

//将string类型的字符串转换成Unicode编码的wstring字符串
wstring StrToUnicode(const string& str, CodeType code_type)
{
	wchar_t str_unicode[256]{ 0 };
	int max{ 0 };
	if (code_type == CodeType::ANSI)
	{
		max = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if (max > 255) max = 255;
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str_unicode, max);
	}
	else
	{
		max = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		if (max > 255) max = 255;
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, str_unicode, max);
	}
	return wstring{ str_unicode };
}

//将Unicode编码的wstring字符串转换成string类型的字符串
string UnicodeToStr(const wstring& wstr, CodeType code_type)
{
	char str[256]{ 0 };
	int max{ 0 };
	if (code_type == CodeType::ANSI)
	{
		max = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (max > 255) max = 255;
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, max, NULL, NULL);
	}
	else
	{
		max = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (max > 255) max = 255;
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str, max, NULL, NULL);
	}
	return string{ str };
}

//判断一个字符串是否UTF8编码
bool IsUTF8Bytes(const char* data)
{
	int charByteCounter = 1;  //计算当前正分析的字符应还有的字节数
	unsigned char curByte; //当前分析的字节.
	bool ascii = true;
	for (int i = 0; i < strlen(data); i++)
	{
		curByte = static_cast<unsigned char>(data[i]);
		if (charByteCounter == 1)
		{
			if (curByte >= 0x80)
			{
				ascii = false;
				//判断当前
				while (((curByte <<= 1) & 0x80) != 0)
				{
					charByteCounter++;
				}
				//标记位首位若为非0 则至少以2个1开始 如:110XXXXX...........1111110X 
				if (charByteCounter == 1 || charByteCounter > 6)
				{
					return false;
				}
			}
		}
		else
		{
			//若是UTF-8 此时第一位必须为1
			if ((curByte & 0xC0) != 0x80)
			{
				return false;
			}
			charByteCounter--;
		}
	}
	if (ascii) return false;		//如果全是ASCII字符，返回false
	else return true;
}

wstring GetExePath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	size_t index;
	wstring current_path{ path };
	index = current_path.find_last_of(L'\\');
	current_path = current_path.substr(0, index + 1);
	return current_path;
}
