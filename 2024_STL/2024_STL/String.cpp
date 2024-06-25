//--------------------------------------------------------------
// String.cpp			STL의 동작을 관찰하기 위한 클래스
// 2024. 4. 4 시작
// 2024. 4. 4 관찰 스위치 추가
// 2024. 5. 2 operater< (list::merge) 
//--------------------------------------------------------------
#include <algorithm>
#include "String.h" // 내가 만든 헤더는 마지막에 사용해야 함

bool 관찰{ false };
size_t String::uid{}; // 클래스 전역변수 초기화



// 이 클래스는 String() 과 ~String()을 코딩할 이유가 전혀 없지만, 관찰하려고 코딩한다.
String::String()
	: id{++uid}
{
	if(관찰)
		std::cout << "[" << id << "] 디폴트 생성, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;
}

String::~String()
{
	if (관찰)
		std::cout << "[" << id << "] 소멸, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;

}




String::String(const char* s)
	: len{ strlen(s) }, id{ ++uid }
{
	p.reset(new char[len]);
	memcpy(p.get(), s, len);

	if (관찰)
		std::cout << "[" << id << "] 생성(char*), 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;

}

//복사 생성자/복사할당 연산자 - 2024. 4. 4
String::String(const String& other)
	: len{ other.len }, id{ ++uid }
{
	p = std::make_unique<char[]>(len);
	memcpy(p.get(), other.p.get(), len);

	if (관찰)
		std::cout << "[" << id << "] 복사 생성, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;

}

String& String::operator=(const String& rhs)
{
	if (this == &rhs)
		return *this;

	len = rhs.len;
	p.release();
	p = std::make_unique<char[]>(len);
	memcpy(p.get(), rhs.p.get(), len);

	if (관찰)
		std::cout << "[" << id << "] 복사 할당 연산자, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;


	return *this;
}

//이동생성자/이동할당연산자 - 2024. 4. 11
// 이동생성자
// noexcept 보증 - 2024. 4. 15
String::String(String&& other) noexcept
	: len{ other.len }, id{++uid}
{
	p = move(other.p); //p = static_cast<String&&>(other.p); //같은 표현임
	other.len = 0;
	other.p.reset();

	if (관찰)
		std::cout << "[" << id << "] 이동 생성 연산자, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;

}
// 이동할당연산자
// noexcept 보증 - 2024. 4. 15
String& String::operator=(String&& rhs) noexcept
{
	if (this == &rhs)
		return *this;

	len = rhs.len;
	p = move(rhs.p);

	rhs.len = 0;
	rhs.p.reset();

	if (관찰)
		std::cout << "[" << id << "] 이동 할당 연산자, 개수: " << len
		<< ", 주소: " << static_cast<void*>(p.get()) << std::endl;

	return *this;
}

//2024. 4.18 operator==
bool String::operator==(const String& rhs) const
{
	if (len != rhs.len) // 글자수로 먼저 같은지 판별
		return false;

	return std::equal(p.get(), p.get() + len, rhs.p.get());

	//Equal 알고리즘을 이용하라
	//for (int i = 0; i < len; ++i)
	//	if (p.get()[i] != rhs.p.get()[i])
	//		return false;


}

// 2024. 5. 2 operater<
bool String::operator<(const String& rhs) const
{
	// 내 String과 rhs의 String을 사전식으로 비교해야 한다.
	// 사전식으로 정렬해주는 함수 lexicographical_compare

	return std::lexicographical_compare(p.get(), p.get() + len,
									rhs.p.get(), rhs.p.get() + rhs.len);
}

bool String::operator>(const String& rhs) const
{
	return std::lexicographical_compare(p.get(), p.get() + len,
									rhs.p.get(), rhs.p.get() + rhs.len);
}



//2024. 4. 4 get/set
size_t String::getLen() const
{
	return len;
}


//2024. 4. 8
std::istream& operator>>(std::istream& is, String& s)
{
	std::string ts;
	is >> ts;
	s.len = ts.size();
	s.p = std::make_unique<char[]>(s.len);
	memcpy(s.p.get(), ts.data(), s.len);
	return is;
}

