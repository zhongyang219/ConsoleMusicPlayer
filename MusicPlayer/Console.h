//************************************************************
//				本文件为控制台相关全局函数的定义
//************************************************************
#pragma once
#include<Windows.h>
#include"Common.h"

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

//计算一个宽字节字符串的长度占用的半角字符数
size_t WcharStrHalfWidthLen(const wchar_t* str)
{
	size_t size{ 0 };
	const size_t length{ wcslen(str) };
	for (int i{ 0 }; i < length; i++)
	{
		if (str[i] >= 0 && str[i] < 128)
			size++;		//如果一个Unicode字符编码在0~127范围内，它占一个半角字符宽度
		else
			size += 2;		//否则它占两个半角字符宽度
	}
	return size;
}

//计算一个宽字节字符串的长度（但是限定最多max_len个半角字符）
size_t WcharStrLen(const wchar_t* str, size_t max_len)
{
	size_t count{ 0 };
	size_t half_width_count{ 0 };
	const size_t length{ wcslen(str) };
	for (; count < length; count++)
	{
		if (str[count] >= 0 && str[count] < 128)
			half_width_count++;
		else
			half_width_count += 2;
		if (half_width_count > max_len) break;
	}
	return count;
}

//计算一个宽字节字符串到第end个半角字符结束时一个有多少个全角字符
size_t FullWidthCount(const wchar_t* str, size_t end)
{
	int half_width_cnt{ 0 }, full_width_count{ 0 };
	size_t length{ wcslen(str) };
	for (int i{ 0 }; i < length; i++)
	{
		if (str[i] < 0 || str[i] >= 128)
		{
			half_width_cnt += 2;
			full_width_count++;
		}
		else
		{
			half_width_cnt++;
		}
		if (half_width_cnt >= end) break;
	}
	return full_width_count;
}

//在界面的x,y坐标处输出一个数字
void PrintInt(int num, short x, short y, Color color)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	char str[20];
	_itoa_s(num, str, 10);
	size_t len{ strlen(str) };
	WriteConsoleOutputCharacterA(handle, str, len, pos, &unuse);		//输出字符
	FillConsoleOutputAttribute(handle, color, len, pos, &unuse);		//设置颜色
}

//在控制台的x,y坐标处输出一个宽字符串
void PrintWString(const wchar_t *str, int x, int y, Color color)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	size_t len{ wcslen(str) };	//字符串长度
	size_t len_halfwidth{ WcharStrHalfWidthLen(str) };	//字符串占用半角字符数
	WriteConsoleOutputCharacterW(handle, str, len, pos, &unuse);			//输出字符
	FillConsoleOutputAttribute(handle, color, len_halfwidth, pos, &unuse);	//设置颜色
}

//在控制台的x,y坐标处输出一个指定最大长度的宽字符串
void PrintWString(const wchar_t* str, short x, short y, size_t length, Color color)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	size_t len{ WcharStrLen(str, length) };		//字符串长度
	size_t len_halfwidth{ WcharStrHalfWidthLen(str) };	//字符串占用半角字符数
	if (len_halfwidth > length) len_halfwidth = length;
	//COORD pos2;
	//pos2.X = x + len_halfwidth - 1;
	//pos2.Y = y;
	//WriteConsoleOutputCharacterA(handle, " ", 1, pos2, &unuse);				//输出字符前先在字符串最后一个半角字符的位置输出一个空格
	WriteConsoleOutputCharacterW(handle, str, len, pos, &unuse);			//输出字符
	FillConsoleOutputAttribute(handle, color, len_halfwidth, pos, &unuse);	//设置颜色
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色
void PrintWString(const wchar_t* str, short x, short y, int split, Color color1, Color color2)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	size_t len{ wcslen(str) };	//字符串长度
	size_t len_halfwidth{ WcharStrHalfWidthLen(str) };	//字符串占用半角字符数
	if (split > len_halfwidth) split = len_halfwidth;
	WriteConsoleOutputCharacterW(handle, str, len, pos, &unuse);		//输出字符
	FillConsoleOutputAttribute(handle, color1, split, pos, &unuse);		//设置颜色1
	pos.X += split;
	FillConsoleOutputAttribute(handle, color2, len_halfwidth - split, pos, &unuse);	//设置颜色2
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色。同时指定最大长度为length
void PrintWString(const wchar_t* str, short x, short y, size_t length, int split, Color color1, Color color2)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	size_t len{ WcharStrLen(str, length) };	//字符串长度
	size_t len_halfwidth{ WcharStrHalfWidthLen(str) };	//字符串占用半角字符数
	if (len_halfwidth > length) len_halfwidth = length;
	if (split > len_halfwidth) split = len_halfwidth;
	//COORD pos2;
	//pos2.X = x + len_halfwidth - 1;
	//pos2.Y = y;
	//WriteConsoleOutputCharacterA(handle, " ", 1, pos2, &unuse);			//输出字符前先在字符串最后一个半角字符的位置输出一个空格
	WriteConsoleOutputCharacterW(handle, str, len, pos, &unuse);		//输出字符
	FillConsoleOutputAttribute(handle, color1, split, pos, &unuse);		//设置颜色1
	pos.X += split;
	FillConsoleOutputAttribute(handle, color2, len_halfwidth - split, pos, &unuse);	//设置颜色2
}

//清除控制台的x,y处开始的length个字符
void ClearString(short x, short y, size_t length)
{
	COORD pos;
	ULONG unuse;
	pos.X = x;
	pos.Y = y;
	//FillConsoleOutputAttribute(handle, BLACK, length, pos, &unuse);		//将要清除的字符的区域设置为黑色
	string mask(length, ' ');		//生成length长度的空格
	WriteConsoleOutputCharacterA(handle, mask.c_str(), length, pos, &unuse);	//将要清除的区域用空格填充
}

//光标移动到x,y坐标
void GotoXY(short x, short y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle, pos);
}

//显示或隐藏光标
void CursorVisible(bool visible)
{
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = visible;
	cci.dwSize = 25;		//光标厚度，取值为1~100
	SetConsoleCursorInfo(handle, &cci);
}

//获取当前控制台窗口的宽度
int GetWindowWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	return info.srWindow.Right + 1;
}

//获取当前控制台窗口的高度
int GetWindowHight()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	return info.srWindow.Bottom + 1;
}
