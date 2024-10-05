# C++ 学习笔记


```
#include <stdio.h>

class Base
{
public:
	Base() { printf("This is Base constructor\n"); }
	~Base() { printf("This is Base destructor\n"); }
};

class Derived : public Base
{
public:
	Derived() { printf("This is Derived constructor\n"); }
	~Derived() { printf("This is Derived destructor\n"); }

};

int main(int argc, char* argv[])
{
	Derived* d = new Derived();
	if(nullptr != d)
	{
		Base* b = d;
		delete b;
	}

	return 0;
}
```

```
#include <stdio.h>

class Base
{
public:
	Base() { printf("This is Base constructor\n"); }
	virtual ~Base() { printf("This is Base destructor\n"); }
};

class Derived : public Base
{
public:
	Derived() { printf("This is Derived constructor\n"); }
	~Derived() { printf("This is Derived destructor\n"); }

};

int main(int argc, char* argv[])
{
	Derived* d = new Derived();
	if(nullptr != d)
	{
		Base* b = d;
		delete b;
	}

	return 0;
}

```
