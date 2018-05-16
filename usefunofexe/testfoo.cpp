#include<stdio.h>
#include<windows.h>
int foo(int a,int b){
	return a+b;
}

int main(){
	printf("input two number:");
	int a=0, b=0 , c=0;
	scanf("%d %d",&a,&b);
	c=foo(a,b);
	printf("result is :%d\n",c);
	system("PAUSE");
	return 0;
}