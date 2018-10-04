

#include <iostream>

#include "../litersa/NTL/zz.h"

using namespace std;
using namespace NTL;
#include "minirsa.h"

#include <windows.h>
#pragma comment(lib,"winmm.lib")

void printzz(NTL::ZZ z)
{
	cout<<z<<endl;
}

template <int B>void printmp(multiple<B> m)
{
	if(m.num[m.words-1]&0x8000)
	{
		multiple<B> c=0;
		c.Sub(m);
		m=c;
		cout<<"-";
	}
	multiple<B> s=10,div;
	char num[1024];
	int k=0;
	while(m!=0)
	{
		m.divrem(s,div);
		int c=m.num[0]+'0';
		m=div;
		num[k]=c;
		k++;
	}
	while(k--)
        cout<<num[k];
	cout<<endl;
}



template <typename B>void mptozz(ZZ &a,B &b)
{
	a=0;
	for(int i=b.words;i--;)
	{
		a<<=16;
		a+=b.num[i];
	}
}

int main()
{
	timeBeginPeriod(1);
	Sleep(1);

	//multiple<1024> wmc,wmb;
	//wmc=0x12345678;
	//wmb=0x12345678;
	//for(int i=0;i<5;i++)
	//{
	//	wmc*=wmc;
	//	wmb=wmb.SpreadSquare();
	//}
	//printmp(wmc);
	//printmp(wmb);


	DWORD tm;
	multiple<256> ma,mb,mc,md;
	ZZ za,zb,zc,zd;
	multiple<128> mq,mp;
	multiple<256> mqp,ml,mim,mgcd;
	multiple<256> me;
	ZZ zq,zp,zqp,zim,zgcd;
	ZZ ze;
	//å›Ç¢Ç…ëfÇ≈Ç†ÇÍÇŒâΩÇ≈Ç‡ó«Ç¢
	me=65537;
	ze=65537;
	char moto[128];
	for(int i=0;i<128;i++)
	{
		if(i<26)
			moto[i]='A'+i;
		else
			moto[i]=0;
	}

	char key[128];

	minirsa<512> rsa,enrsa;
	rsa.GenerateKey();
	rsa.Export(key);

	enrsa.Import(key);
	enrsa.Encode(moto);

	rsa.Decode(moto);

	tm=timeGetTime();
//	for(;;)
	{
	mp.GeneratePrime(128);
	mq.GeneratePrime(128);

	//mp="136576110845647384328386098206330041799";
	//mq="153917437308658625042075100475607846393";

	//p,q,nê∂ê¨
	mptozz(zq,mq);
	cout<<zq<<endl;
	mptozz(zp,mp);
	cout<<zp<<endl;
	cout<<timeGetTime()-tm<<" msec generate prime"<<endl;
	tm=timeGetTime();

	cout<<"RSA start..."<<endl;

	tm=timeGetTime();

	mqp.Mul(mq,mp);
	printmp(mq);
	printmp(mp);
	printmp(mqp);

	//îÈñßåÆdê∂ê¨
	mp.Dec();
	mq.Dec();
	mim.Mul(mp,mq);
	printmp(mim);
	md.modinv(me,mim);
	printmp(md);
	for(int i=0;i<sizeof(ma);i++)
	{
		((char*)(&ma))[i]=moto[i];
	}
	printmp(ma);
	ma.powermod(me,mqp);
	printmp(ma);
	ma.powermod(md,mqp);
	printmp(ma);

	cout<<timeGetTime()-tm<<" msec"<<endl;
	tm=timeGetTime();

	//n=p*n
	zqp=zq*zp;
	printzz(zq);
	printzz(zp);
	printzz(zqp);

	//l=o(n)
	zim=(zp-1)*(zq-1);
	printzz(zim);

	//d
	InvModStatus(zd,ze,zim);
	printzz(zd);

	//data
	za=0;
	for(int i=0;i<32;i++)
	{
		za*=256;
		za+=moto[31-i];
	}
	printzz(za);

	zb=PowerMod(za,ze,zqp);
	printzz(zb);

    za=PowerMod(zb,zd,zqp);
	printzz(za);

	//ç◊Ç©Ç¢ë¨ìx
	cout<<timeGetTime()-tm<<" msec"<<endl;
	tm=timeGetTime();
	}
	multiple<256> mult,mulp;
	mult.Random(256);

	multiple<128> omp;
	for(int i=0;i<10;i++)
	{
		omp.GeneratePrime(128);
	}
	cout<<timeGetTime()-tm<<" msec(128bit prime)"<<endl;
	tm=timeGetTime();
	for(int i=0;i<1000000;i++)
	{
//		mulp=i;
		mult.LeftShift();
		mult.RightShift();
	}
	cout<<timeGetTime()-tm<<" msec(shift)"<<endl;
	tm=timeGetTime();
	for(int i=0;i<100000;i++)
	{
		mult.Square(mult);
	}

	cout<<timeGetTime()-tm<<" msec(square)"<<endl;
	tm=timeGetTime();
	for(int i=0;i<100000;i++)
	{
		mult.Mul(omp,omp);
        
	}
	cout<<timeGetTime()-tm<<" msec(mul)"<<endl;
	tm=timeGetTime();
	for(int i=0;i<10;i++)
	{
		zb=PowerMod(za,ze,zqp);
		za=PowerMod(zb,zd,zqp);
	}

	cout<<timeGetTime()-tm<<" msec(powermodZZ)"<<endl;
	tm=timeGetTime();

	for(int i=0;i<10;i++)
	{
		ma.PowerMod(me,mqp);
		ma.PowerMod(md,mqp);
	}

	cout<<timeGetTime()-tm<<" msec(powermod)"<<endl;
	tm=timeGetTime();

//	ëfêîê∂ê¨Ç∆ëfàˆêîï™âÉeÉXÉg
//	unsigned a,b,c;
//	for(int i=0;i<6000;i++)
//	{
//		ma.GeneratePrime(60);
////		ma.GeneratePrime(128);
//		printmp(ma);
//		mb=ma;
//		mb>>=30;
//		bool good=1;a=(*(int*)(&mb));
//		for(b=3;b<a;b+=2)
//		{
//            c=(*(__int64*)(&ma))%b;
//			if(c==0)
//			{
//				cout<<b<<" ";
//				good=0;
//			}
//		}
//		cout<<endl;
//	}
	int qqq;
	cin>>qqq;
	return 0;
}
