//--------------------------------------------------------------
// String.cpp			STL�� ������ �����ϱ� ���� Ŭ����
// 2024. 4. 4 ����
// 2024. 4. 4 ���� ����ġ �߰�
// 2024. 5. 2 operater< (list::merge) 
//--------------------------------------------------------------
#include <algorithm>
#include "String.h" // ���� ���� ����� �������� ����ؾ� ��

bool ����{ false };
size_t String::uid{}; // Ŭ���� �������� �ʱ�ȭ



// �� Ŭ������ String() �� ~String()�� �ڵ��� ������ ���� ������, �����Ϸ��� �ڵ��Ѵ�.
String::String()
	: id{++uid}
{
	if(����)
		std::cout << "[" << id << "] ����Ʈ ����, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;
}

String::~String()
{
	if (����)
		std::cout << "[" << id << "] �Ҹ�, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;

}




String::String(const char* s)
	: len{ strlen(s) }, id{ ++uid }
{
	p.reset(new char[len]);
	memcpy(p.get(), s, len);

	if (����)
		std::cout << "[" << id << "] ����(char*), ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;

}

//���� ������/�����Ҵ� ������ - 2024. 4. 4
String::String(const String& other)
	: len{ other.len }, id{ ++uid }
{
	p = std::make_unique<char[]>(len);
	memcpy(p.get(), other.p.get(), len);

	if (����)
		std::cout << "[" << id << "] ���� ����, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;

}

String& String::operator=(const String& rhs)
{
	if (this == &rhs)
		return *this;

	len = rhs.len;
	p.release();
	p = std::make_unique<char[]>(len);
	memcpy(p.get(), rhs.p.get(), len);

	if (����)
		std::cout << "[" << id << "] ���� �Ҵ� ������, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;


	return *this;
}

//�̵�������/�̵��Ҵ翬���� - 2024. 4. 11
// �̵�������
// noexcept ���� - 2024. 4. 15
String::String(String&& other) noexcept
	: len{ other.len }, id{++uid}
{
	p = move(other.p); //p = static_cast<String&&>(other.p); //���� ǥ����
	other.len = 0;
	other.p.reset();

	if (����)
		std::cout << "[" << id << "] �̵� ���� ������, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;

}
// �̵��Ҵ翬����
// noexcept ���� - 2024. 4. 15
String& String::operator=(String&& rhs) noexcept
{
	if (this == &rhs)
		return *this;

	len = rhs.len;
	p = move(rhs.p);

	rhs.len = 0;
	rhs.p.reset();

	if (����)
		std::cout << "[" << id << "] �̵� �Ҵ� ������, ����: " << len
		<< ", �ּ�: " << static_cast<void*>(p.get()) << std::endl;

	return *this;
}

//2024. 4.18 operator==
bool String::operator==(const String& rhs) const
{
	if (len != rhs.len) // ���ڼ��� ���� ������ �Ǻ�
		return false;

	return std::equal(p.get(), p.get() + len, rhs.p.get());

	//Equal �˰����� �̿��϶�
	//for (int i = 0; i < len; ++i)
	//	if (p.get()[i] != rhs.p.get()[i])
	//		return false;


}

// 2024. 5. 2 operater<
bool String::operator<(const String& rhs) const
{
	// �� String�� rhs�� String�� ���������� ���ؾ� �Ѵ�.
	// ���������� �������ִ� �Լ� lexicographical_compare

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

