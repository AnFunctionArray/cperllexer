#include <stack>

std::stack<int> condstack;

extern "C" void push_condstack(int val)
{
	condstack.push(val);
}

extern "C" void pop_condstack()
{
	condstack.pop();
}

extern "C" int top_condstack()
{
	return condstack.top();
}

extern "C" int size_condstack()
{
	return condstack.size();
}