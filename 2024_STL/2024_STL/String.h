//--------------------------------------------------------------
// String.h				STL�� ������ �����ϱ� ���� Ŭ����
// 2024. 4. 4 ����
// 2024. 4. 4 Ŭ������ special ������ �����Ѵ�.
// 2024. 5. 2 operator< (list::merge)
// 2024. 5. 6 begin, end
// 2024. 5. 9 rbegin, rend �� ���� Ÿ���� Ŭ������ �ڵ��ؾ� �Ѵ�
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

	// �ݺ����� �⺻ �������̽� 
	String_iterator& operator++() {
		++p;
		return *this;
	};

	char& operator*() const { // sort ������ const ����
		return *(p);
	};

	bool operator==(const String_iterator& rhs) const {
		return p == rhs.p;
	};

	// std:: sort�� ����ϴ� �����ڸ� �ڵ��Ѵ�.
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

// ������ �ݺ��ڸ� �ڵ��� ������ ����. make_reverse_iterator�� ����Ѵ�.
// �׷����� �����̳��� �ݺ��ڴ� �ݵ�� class�� �ڵ��ؾ� �Ѵ�.

// 2024. 5. 9 String�� ������ �ݺ��� ���� �ڵ� - �ݺ��� ������� ���� 
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

	// �ݺ����� �⺻ �������̽� 
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
	// �� Ŭ������ String() �� ~String()�� �ڵ��� ������ ���� ������, �����Ϸ��� �ڵ��Ѵ�.
	String();
	~String();


	String(const char* s);

	//���� ������/�����Ҵ� ������ - 2024. 4. 4
	String(const String& other);

	//�̵�������/�̵��Ҵ翬���� - 2024. 4. 11
	// noexcept ���� - 2024. 4. 15
	String(String&& other) noexcept; // �� �Լ��� �����ϴ� ����, exception�� throw ���� ���� �Ŷ�� ����
	String& operator=(String&& rhs) noexcept; // ������ throw �ϸ� ����

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
	static size_t uid;		//�뵵�� Ȯ������ // private������ ����������// ����? ���ϱ�?



};