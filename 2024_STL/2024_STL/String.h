//--------------------------------------------------------------
// String.h				STL의 동작을 관찰하기 위한 클래스
// 2024. 4. 4 시작
// 2024. 4. 4 클래스의 special 동작을 관찰한다.
// 2024. 5. 2 operator< (list::merge)
// 2024. 5. 6 begin, end
// 2024. 5. 9 rbegin, rend 의 리턴 타입을 클래스로 코딩해야 한다
//--------------------------------------------------------------
#pragma once
#include <memory>
#include <iostream>

class String_iterator {
public:
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = char*;
	using reference = char&;
	using iterator_category = std::random_access_iterator_tag;

	// C++20
	using iterator_concept = std::contiguous_iterator_tag;

private:
	char* p;

public:
	String_iterator(char* p) : p{ p } {};

	// 반복자의 기본 인터페이스 
	String_iterator& operator++() {
		++p;
		return *this;
	};

	char& operator*() const { // sort 때문에 const 붙임
		return *(p);
	};

	bool operator==(const String_iterator& rhs) const {
		return p == rhs.p;
	};

	// std:: sort가 사용하는 연산자를 코딩한다.
	difference_type operator-(const String_iterator& rhs) const {
		return p - rhs.p;
	}

	String_iterator& operator--() {
		--p;
		return *this;
	}

	String_iterator operator+(difference_type diff) const {
		return String_iterator{ p + diff };
	}

	bool operator<(const String_iterator& rhs) const {
		return p < rhs.p;
	}

	String_iterator operator-(difference_type diff) const {
		return String_iterator{ p - diff };
	}
};

// 역방향 반복자를 코딩할 이유는 없다. make_reverse_iterator를 사용한다.
// 그렇지만 컨테이너의 반복자는 반드시 class로 코딩해야 한다.

// 2024. 5. 9 String의 역방향 반복자 직접 코딩 - 반복자 어댑터의 일종 
class String_reverse_iterator {
public:
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = char*;
	using reference = char&;
	using iterator_category = std::random_access_iterator_tag;

	// C++17
	using iterator_concept = std::contiguous_iterator_tag;

private:
	char* p;

public:
	String_reverse_iterator(char* p) : p{ p } {};

	// 반복자의 기본 인터페이스 
	String_reverse_iterator& operator++() {
		--p;
		return *this;
	};

	char& operator*() {
		return *(p - 1);
	};

	bool operator==(const String_reverse_iterator& rhs) const {
		return p == rhs.p;
	};

};

class String {
	size_t len{};
	std::unique_ptr<char[]> p{};
	size_t id{};

public:
	// 이 클래스는 String() 과 ~String()을 코딩할 이유가 전혀 없지만, 관찰하려고 코딩한다.
	String();
	~String();


	String(const char* s);

	//복사 생성자/복사할당 연산자 - 2024. 4. 4
	String(const String& other);

	//이동생성자/이동할당연산자 - 2024. 4. 11
	// noexcept 보증 - 2024. 4. 15
	String(String&& other) noexcept; // 이 함수를 실행하는 동안, exception을 throw 하지 않을 거라고 선언
	String& operator=(String&& rhs) noexcept; // 하지만 throw 하면 망함

	//2024. 4.18 operator==
	bool operator==(const String& rhs) const;



	//2024. 4. 4 get/set
	size_t getLen() const;


	friend std::ostream& operator<<(std::ostream& os, const String& s) {
		for (size_t i = 0; i < s.len; ++i)
			os << s.p.get()[i];
		return os;
	}

	String& operator=(const String& rhs);	

	// 2024. 5. 6 begin/end 
	String_iterator begin() const {
		return String_iterator{ p.get() };
	}
	// 2024. 5. 6 begin/end 
	String_iterator const end() const {
		return String_iterator{ p.get() + len };
	}


	// 2024. 5. 9 rbegin/rend
	String_reverse_iterator rbegin() const {
		return String_reverse_iterator{ p.get() + len };
	}
	// 2024. 5. 9 rbegin/rend	
	String_reverse_iterator rend() const {
		return String_reverse_iterator{ p.get() };
	}


	// 2024. 4. 8
	friend std::istream& operator>>(std::istream& is, String& s);

	// 2024. 5. 2 operater<
	bool operator<(const String& rhs) const;
	bool operator>(const String& rhs) const;


private:
	static size_t uid;		//용도를 확인하자 // private이지만 전역변수임// 뭘까? 왜일까?



};