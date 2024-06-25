/*-----------------------------------------------------------------------------------
 2024. 06. 10 �� 56  											(14��/2��)

 �� �б� ��� ���� "������"



 ���ǿ����� �ٰŸ��� �̾߱� �ϴ°�, å�� ���� ��� �˾ƺ��ƾ� ���°� �ִ�.
 
 ���δ� �ݺ��̴�.

 ===========================
 6�� 13��(15�� 2��) - �⸻ ���
 ===========================
 -----------------------------------------------------------------------------------*/
#include <iostream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <ranges>
#include <random>
#include <print>
#include "save.h"
#include "String.h"
//using namespace std;

extern bool ����; // �����Ϸ��� true��



//--------
int main()
//--------
{
	//==================================================
	save_problem("FileName.cpp", "15�� 1���� ����.txt");
	save("FileName.cpp", "STL - 06 - 10 ���� ����.txt");
	//==================================================

	/*
		C++: 
			OOP: 1�г� �� ��� ��
			Generic PL : �̹� �б�
			Function PL: �̹� �б�

		class String -> OOP ����
			String���� 6���� special function ����
						Default �����ڸ� ����� �Լ���
						������
						���� ������
						���� �Ҵ��� =
						�̵� ������
						�̵� �Ҵ��� =

				RAII - �ڿ� �Ҹ� �ڵ�ȭ

				// ���� �����ϰ� ������ ������ �� Ȱ������

				�޸� ������ ������ ���� �Ǵ��� �ٷ� ����� �� �ִ� ������ �Ǿ�� �Ѵ�.
				- Local			: Stack
				- Global		: Data
				- new String	: Free-Store
				
				// new�� ���� delete ��� �ϴ�
				// unique_ptr�� �ڵ�ȭ ����

	*/

	/*
		STL Container

		1. Sequence
			- array<T, N> 
				// T[N] ����� �迭�� ������ ���� ����
				// array<T, N>�� ���� �迭 ������ �޸� ��ġ, ������ �Ϻ��� ��ü�Ѵ�.
				// ������ ����ϸ�, ũ�⿡ ���� ������ ��������, ������ ����ϸ�, �ʱ�ȭ�� ���� ��쿡�� ���Ϳ� ����.
			- vector<T>
			- deque<T>
			- list<T>
			- forward_list<T>


		array , vector, deque --> span<T> �� ���� �� ����
	
		
		2. Associative
			- Set<key>
			- multiset<key>
			- map<key, value>
			- multimap<key, value>


		3. Unordered Associative
			- unordered_set<k>
			- unordered_multiset<k>
			- unordered_map<k>
			- unordered_multimap<k,v>

			
		1,2,3 �����̳� Ÿ���� ���� �˻� ��Ŀ� ���� �з��� ��
		1. ���Ҹ� ���������� ��	: O(n)
		2. ���� Ž��				: O(log n)
		3.						: O(1)

		�� ��:
		Container Adaptor
		- Stack, Queue, flat_set, flat_map
	*/

	/*
		Iterator - �ݺ���
		
		�ݺ��ڴ� �����̳ʰ� ���� �ش�.

		�ݺ��ڴ� ������ �ִ�. - Category

		Category:	���� ����(hierarchy)
			- input_iterator_tag
			- output_iterator_tag
			- forward_iterator_tag
			- bidirectional_iterator_tag
			- random_access_iterator_tag

			- contiguous_iterator_tag

		class String_iterator {
			iterator-traits �׸�.

			using iterator_category
				= random_access_iterator_tag
			...
		};

		class String_Reverse_iterator {
		 *, ++, == --> �ݺ��� ���, �ӿ��� �� ���� ��

		};

		�츮�� ������� �� Ȯ���غ���
	*/

	/*
	Algorithm
	
	1. Non-Modifying Sequence Operation
	2. Modifying(Mutating) Sequence Operation
	3. Sort and Related Operation

	�츰 distance �Լ� ���� �غ�
		- �ݺ��� ������ ���� �ٸ� ����� ���� 
		- concept���� dispatch // Template �Լ� ���ڸ� ���� �Լ���  dispatch �� �� �ִ�.
			��-> ranges::algorithm == constrained algorithm

		constrained algorithm = template ���ڸ� concept���� �ۼ�

		view �� ���� -> functional programming language�� ����� �� �ְ� ��
	
	*/

	/*

	STL 3���� ���
		- container
		- iterator
		- algorithm


	���������� ���α׷��� ���Ǹ� �� �ڴ�� �ϱ�� �ſ� �����
	���α׷����� ������ �ڷḦ �б⸸ �ϸ� ���ΰ� �ȵ�, 
	�ǽ��غ��� ���� �ڵ��غ���, �������� ��
	�ڵ��� �ð��� ���� �� ����� ���ϰ� �Ǿ� ����, "���"
	������ �ʺ��� �ڵ��� ���Ѵٸ�, �� ����� �ʺ��� �� ���ڰ� �� ���� �ڵ��� �ѰŴ�.
	�ڵ��� ���� �õ��ϰ� Ʋ�� ���� ���ϰ� �� -> �Ŀ�, Ʋ�� �ð��� ���� �� ����

	


	STL å ��õ
	 - C++20 // ǳ���� ������ ������ �ٽ� ���


	*/
}
