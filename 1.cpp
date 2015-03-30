#include<iostream>
using namespace std;
int m,n;
const int N=100;
int num[N][N];
int fun(int i,int j)
{
	if(i==m&&j==n)
		return 1;
	else if(i>m||j>n)
		return 0;
	if(num[i+1][j]==-1)
		num[i+1][j]=fun(i+1,j);
	if(num[i][j+1]==-1)
		num[i][j+1]=fun(i,j+1);
	return num[i+1][j]+num[i][j+1];
}
int main()
{
	int i,j;
	cin>>m>>n;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			num[i][j]=-1;
	cout<<fun(0,0)<<endl;
	return 0;
}
