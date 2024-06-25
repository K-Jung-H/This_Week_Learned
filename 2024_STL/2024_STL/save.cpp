//----------------------------------------------------------------------
// save.cpp �� �б� ���Ǹ� �����Ѵ�
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

	// 1. ���� ������ ����
	// 2. �� ������ �����̱� ���� ����
	// 3. ���� ������ ��� ������ �о� �� ���Ͽ� �����δ�


	//���� ������ ����
	ifstream read_file{ fileName.data() };

	if (!read_file)
	{
		cout << "������ ���� ���߽��ϴ�.\n";
		exit(0);
	}

	//�� ������ �����̱� ���� ����
	ofstream out(Write_file.data(), ios::app);
	

	//���� ������ ��� ������ �о� �� ���Ͽ� �����δ�
	vector<char> v{ istreambuf_iterator<char>{read_file},{} };

	//���� ���� ���
	//for (char c : v)
	//	cout << c;

	//������ �ð��� ���Ͽ� ����Ѵ�.
	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now); // utc���� ��ȯ
	auto lt = localtime(&time); // ���� �ð����� ��ȯ

	auto old = out.imbue(locale("ko_KR"));


	out.imbue(old);  //�۾��� ������ ���� ���¸� ���� �����쿡�� �귯�� ����ϰ� ���� �귯�÷� �ǵ����� ����� ����
	out << '\n' << '\n';
	out << "================================================" << '\n';
	out << fileName << put_time(lt, " , ���� �ð�: %x %A %X") << '\n';
	out << "================================================" << '\n';


	//STL�� Ȱ���� ���� ���
	copy(v.begin(), v.end(), ostreambuf_iterator<char>{ out });


	cout << "================================================" << '\n';
	cout << "���������� " << fileName.data() << "������ " << Write_file.data() << "�� �����Ͽ���" << endl;
	cout << "================================================" << '\n';
}


void save_problem(string_view fileName, string_view Write_file)
{

	// 1. ���� ������ ����
	// 2. �� ������ �����̱� ���� ����
	// 3. ���� ������ ��� ������ �о� �� ���Ͽ� �����δ�


	//���� ������ ����
	ifstream read_file{ fileName.data() };

	if (!read_file)
	{
		cout << "������ ���� ���߽��ϴ�.\n";
		exit(0);
	}

	//�� ������ �����̱� ���� ����
	ofstream out(Write_file.data(), ios::app);


	//���� ������ ��� ������ �о� �� ���Ͽ� �����δ�
	vector<char> v{ istreambuf_iterator<char>{read_file},{} };

	//���� ���� ���
	//for (char c : v)
	//	cout << c;

	//������ �ð��� ���Ͽ� ����Ѵ�.
	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now); // utc���� ��ȯ
	auto lt = localtime(&time); // ���� �ð����� ��ȯ

	auto old = out.imbue(locale("ko_KR"));


	out.imbue(old);  //�۾��� ������ ���� ���¸� ���� �����쿡�� �귯�� ����ϰ� ���� �귯�÷� �ǵ����� ����� ����
	out << '\n' << '\n';
	out << "================================================" << '\n';
	out << fileName << put_time(lt, " , ���� �ð�: %x %A %X") << '\n';
	out << "================================================" << '\n';


	//STL�� Ȱ���� ���� ���
	copy(v.begin(), v.end(), ostreambuf_iterator<char>{ out });


	//ȿ���� ���� ���
	//for (char c : v)
	//	out << c;

	cout << "���������� " << Write_file.data() << "���Ͽ� �����Ͽ���" << endl;
}