/*-----------------------------------------------------------------------------------
 2024. 06. 10 월 56  											(14주/2일)

 한 학기 배운 내용 "총정리"



 강의에서는 줄거리만 이야기 하는것, 책을 통해 깊게 알아보아야 얻어가는게 있다.
 
 공부는 반복이다.

 ===========================
 6월 13일(15주 2일) - 기말 고사
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

extern bool 관찰; // 관찰하려면 true로



//--------
int main()
//--------
{
	//==================================================
	save_problem("FileName.cpp", "15주 1일차 문제.txt");
	save("FileName.cpp", "STL - 06 - 10 강의 저장.txt");
	//==================================================

	/*
		C++: 
			OOP: 1학년 때 배운 것
			Generic PL : 이번 학기
			Function PL: 이번 학기

		class String -> OOP 복습
			String에는 6가지 special function 존재
						Default 생성자만 스페셜 함수임
						생성자
						복사 생성자
						복사 할당자 =
						이동 생성자
						이동 할당자 =

				RAII - 자원 소멸 자동화

				// 관찰 가능하게 했으니 복습할 때 활용하자

				메모리 영역이 어디어디로 가게 되는지 바로 대답할 수 있는 수준이 되어야 한다.
				- Local			: Stack
				- Global		: Data
				- new String	: Free-Store
				
				// new를 쓰면 delete 써야 하니
				// unique_ptr로 자동화 하자

	*/

	/*
		STL Container

		1. Sequence
			- array<T, N> 
				// T[N] 방식의 배열을 앞으로 쓰지 마라
				// array<T, N>이 기존 배열 구조의 메모리 위치, 구조를 완벽히 대체한다.
				// 지역에 사용하면, 크기에 따라 문제가 생기지만, 전역에 사용하면, 초기화를 안한 경우에는 벡터와 같다.
			- vector<T>
			- deque<T>
			- list<T>
			- forward_list<T>


		array , vector, deque --> span<T> 로 묶을 수 있음
	
		
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

			
		1,2,3 컨테이너 타입은 원소 검색 방식에 따라 분류한 것
		1. 원소를 순차적으로 비교	: O(n)
		2. 이진 탐색				: O(log n)
		3.						: O(1)

		그 외:
		Container Adaptor
		- Stack, Queue, flat_set, flat_map
	*/

	/*
		Iterator - 반복자
		
		반복자는 컨테이너가 만들어서 준다.

		반복자는 종류가 있다. - Category

		Category:	계층 구조(hierarchy)
			- input_iterator_tag
			- output_iterator_tag
			- forward_iterator_tag
			- bidirectional_iterator_tag
			- random_access_iterator_tag

			- contiguous_iterator_tag

		class String_iterator {
			iterator-traits 항목.

			using iterator_category
				= random_access_iterator_tag
			...
		};

		class String_Reverse_iterator {
		 *, ++, == --> 반복자 어뎁터, 속에서 뭘 할지 모름

		};

		우리가 만들었던 걸 확인해보라
	*/

	/*
	Algorithm
	
	1. Non-Modifying Sequence Operation
	2. Modifying(Mutating) Sequence Operation
	3. Sort and Related Operation

	우린 distance 함수 구현 해봄
		- 반복자 종류에 따라 다른 기능을 수행 
		- concept으로 dispatch // Template 함수 인자를 보고 함수를  dispatch 할 수 있다.
			ㄴ-> ranges::algorithm == constrained algorithm

		constrained algorithm = template 인자를 concept으로 작성

		view 와 결합 -> functional programming language로 기능할 수 있게 함
	
	*/

	/*

	STL 3개의 기둥
		- container
		- iterator
		- algorithm


	누군가에게 프로그래밍 강의를 제 멋대로 하기는 매우 어려움
	프로그래밍은 정리된 자료를 읽기만 하면 공부가 안됨, 
	의심해보고 직접 코딩해보고, 느껴봐야 함
	코딩은 시간을 많이 쓴 사람이 잘하게 되어 있음, "언어"
	누군가 너보다 코딩을 잘한다면, 그 사람은 너보다 잠 안자고 더 많이 코딩을 한거다.
	코딩은 많이 시도하고 틀릴 수록 잘하게 됨 -> 후에, 틀린 시간을 줄일 수 있음

	


	STL 책 추천
	 - C++20 // 풍부한 예제로 익히는 핵심 기능


	*/
}
