//----------------------------------------------------------------------
// save.cpp 한 학기 강의를 저장한다
// 
// 2024.03.07
//----------------------------------------------------------------------
#include "save.h"
#include <string_view>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

void save(string_view fileName, string_view Write_file)
{

	// 1. 읽을 파일을 연다
	// 2. 쓸 파일을 덧붙이기 모드로 연다
	// 3. 읽을 파일의 모든 내용을 읽어 쓸 파일에 덧붙인다


	//읽을 파일을 연다
	ifstream read_file{ fileName.data() };

	if (!read_file)
	{
		cout << "파일을 열지 못했습니다.\n";
		exit(0);
	}

	//쓸 파일을 덧붙이기 모드로 연다
	ofstream out(Write_file.data(), ios::app);
	

	//읽을 파일의 모든 내용을 읽어 쓸 파일에 덧붙인다
	vector<char> v{ istreambuf_iterator<char>{read_file},{} };

	//파일 내용 출력
	//for (char c : v)
	//	cout << c;

	//저장한 시간을 파일에 기록한다.
	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now); // utc으로 변환
	auto lt = localtime(&time); // 현지 시간으로 변환

	auto old = out.imbue(locale("ko_KR"));


	out.imbue(old);  //작업이 끝나면 원래 형태를 저장 윈도우에서 브러시 사용하고 이전 브러시로 되돌리는 방법과 동일
	out << '\n' << '\n';
	out << "================================================" << '\n';
	out << fileName << put_time(lt, " , 저장 시간: %x %A %X") << '\n';
	out << "================================================" << '\n';


	//STL을 활용한 저장 방법
	copy(v.begin(), v.end(), ostreambuf_iterator<char>{ out });


	cout << "================================================" << '\n';
	cout << "성공적으로 " << fileName.data() << "파일을 " << Write_file.data() << "에 저장하였음" << endl;
	cout << "================================================" << '\n';
}


void save_problem(string_view fileName, string_view Write_file)
{

	// 1. 읽을 파일을 연다
	// 2. 쓸 파일을 덧붙이기 모드로 연다
	// 3. 읽을 파일의 모든 내용을 읽어 쓸 파일에 덧붙인다


	//읽을 파일을 연다
	ifstream read_file{ fileName.data() };

	if (!read_file)
	{
		cout << "파일을 열지 못했습니다.\n";
		exit(0);
	}

	//쓸 파일을 덧붙이기 모드로 연다
	ofstream out(Write_file.data(), ios::app);


	//읽을 파일의 모든 내용을 읽어 쓸 파일에 덧붙인다
	vector<char> v{ istreambuf_iterator<char>{read_file},{} };

	//파일 내용 출력
	//for (char c : v)
	//	cout << c;

	//저장한 시간을 파일에 기록한다.
	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now); // utc으로 변환
	auto lt = localtime(&time); // 현지 시간으로 변환

	auto old = out.imbue(locale("ko_KR"));


	out.imbue(old);  //작업이 끝나면 원래 형태를 저장 윈도우에서 브러시 사용하고 이전 브러시로 되돌리는 방법과 동일
	out << '\n' << '\n';
	out << "================================================" << '\n';
	out << fileName << put_time(lt, " , 저장 시간: %x %A %X") << '\n';
	out << "================================================" << '\n';


	//STL을 활용한 저장 방법
	copy(v.begin(), v.end(), ostreambuf_iterator<char>{ out });


	//효율적 저장 방법
	//for (char c : v)
	//	out << c;

	cout << "성공적으로 " << Write_file.data() << "파일에 저장하였음" << endl;
}