
template<int bitsize>
class multiple;

template <int B>
void printmp(multiple<B> m);

//#define USING_ASM

template <int bitsize>
class multiple
{
public:
	static const int bits=bitsize;
	static const int words=bits/16;
	static const int dwords=bits/32;
	union{
	unsigned short num[words];
	unsigned int dnum[dwords];
	};
public:
	void Initialize(const char *num)
	{
		int n;
		multiple<bits+32> temp;
		temp=0;
		*this=0;
		while(*num)
		{
			temp.Mul(*this,10);
			*this=temp;
			n=(*num-'0');
			this->Add(n);
			num++;
		}
	}
	template <typename T> inline void Initialize(T n)
	{
		if(sizeof(T)<bits/8)
		{
			int i=0;
			for(i=0;i<sizeof(T)/2;i++)
				num[i]=((unsigned short*)(&n))[i];
			for(;i<words;i++)
				num[i]=0;
		}
		else
		{
			for(int i=0;i<words;i++)
				num[i]=((unsigned short*)(&n))[i];

		}
		//#else
//		static const int tsz=sizeof(n)/4;
//		static const int bsz=bitsize/32-tsz;
//		__asm 
//		{
//			mov edi,dword ptr[this];
//			lea esi,n;
//			mov ecx,tsz;
//			rep movs edi;
//			xor eax,eax;
//			mov ecx,bsz;
//			rep stos eax;
//		}
//#endif
	}

	multiple()
	{
	}
	template <typename T>multiple(T n)
	{
		Initialize(n);
	}
	void Random(int bit)
	{
		for(int i=0;i<bit/8;i++)
		{
			((unsigned char *)(num))[i]=rand()>>2;
		}
		for(int i=bit/32;i<words/2;i++)
		{
			((int*)(num))[i]=0;
		}
	}
	void Inc()
	{
		for(int i=0;i<dwords;i++)
			if(++dnum[i])
				break;
	}
	void Dec()
	{
		for(int i=0;i<dwords;i++)
			if(--dnum[i]!=0xffffffff)
				break;
	}
	//void GeneratePrime(int bit,int loop)
	//{
	//	multiple t,n,a,nm,an;
	//	multiple one=1;
	//	for(;;)
	//	{
	//		n.Random(bit);
	//		n.num[0]|=1;
 //           for(int i=loop;i--;)
	//		{
	//			a.Random(bit/2);
	//			t.gcd(n,a);
	//			nm=n;
	//			nm-=one;
	//			an=a;
	//			an.powermod(nm,n);
	//			if(an==1)
	//			{
	//				if(i==0)
	//				{
	//					*this=n;
	//					n>>=bit;
	//					if(n!=0)
	//						break;
	//					return;
	//				}
	//			}
	//			else if(t<n&&t!=1)
	//			{
 //                   t-=one;
	//				n+=t;
	//				i=loop;
	//			}
	//		}
	//	}
	//}
	void GeneratePrime(int bit)
	{
		multiple n,nm,an;
		multiple<bits/2> a,t;
		for(;;)
		{
			n.Random(bit);
			n.num[0]|=1;
			n.num[words-1]&=0x3fff;
			n.num[words-1]|=0x2000;
			a.Random(bit/2);
			a.num[0]|=1;
			for(;;)
			{
				t.GCD(n,a);
				if(t==1)
					break;
				t.Dec();
				n.Add(t);
			}
			nm=n;
			nm.Dec();
			an=a;
			an.PowerMod(nm,n);
			if(an==1)
			{
				*this=n;
				return;
			}
		}
	}
	template <typename T>multiple& operator =(T n)
	{
		Initialize(n);
		return *this;
	}
	template <typename T> void Add(T &a)
	{
		multiple<sizeof(T)*8> &n=*(multiple<sizeof(T)*8>*)&a;
		unsigned int over=0;
		int i=0;
		for(;i<sizeof(T)/2;i++)
		{
			over=n.num[i]+num[i]+over;
			num[i]=over;
			over>>=16;
		}
		if(sizeof(T)/2<words||over)
			for(;i<words&&over;i++)
			{
				over=num[i]+over;
				num[i]=over;
				over>>=16;
			}
	}
	//multiple& operator += (multiple &n)
	//{
 //       unsigned int over=0;
	//	for(int i=0;i<words;i++)
	//	{
	//		over=n.num[i]+num[i]+over;
 //           num[i]=over;
	//		over>>=16;
	//	}
	//	return *this;
	//}
	template <typename T>void Sub(T &n)
	{
		unsigned int under=0,temp;
		unsigned int *b=(unsigned int*)&n;
		for(int i=0;i<((sizeof(T)/4)<dwords?sizeof(T)/4:dwords);i++)
		{
			if(under&&dnum[i]==b[i])
			{
				dnum[i]=0xffffffff;
				continue;
			}
			temp=dnum[i];
			under=dnum[i]-b[i]-under;
			dnum[i]=under;
			if(under>temp)
                under=1;
			else
				under=0;
		}
		if(under)
		{
			for(int i=sizeof(T)/4;i<dwords&&under;i++)
			{
				temp=dnum[i];
				under=dnum[i]-under;
				dnum[i]=under;
				if(under>temp)
					under=1;
				else under=0;
			}
		}
	}
//	template <typename T>multiple& operator -=(T &n)
//	{
//		unsigned int under=0;
//		unsigned short *a=(unsigned short*)this;
//		unsigned short *b=(unsigned short*)&n;
//		for(int i=0;i<((sizeof(T)/2)<words?sizeof(T)/2:words);i++)
////		for(int i=0;i<sizeof(T)/2;i++)
//		{
//			under=a[i]-b[i]-under;
//			a[i]=under;
//			if(((int)under)<0)
//                under=1;
//			else
//				under=0;
//		}
//		if(under)
//		{
//			for(int i=sizeof(T)/2;i<words&&under;i++)
//			{
//				under=a[i]-under;
//				a[i]=under;
//				if(((int)under)<0)
//					under=1;
//				else under=0;
//			}
//		}
//		return *this;
//	}
	template <typename A,typename B>void Mul(A a,B b)//a*bÅ®this
	{
		unsigned short *anum=(unsigned short *)&a;
		unsigned short *bnum=(unsigned short *)&b;
		unsigned int over=0,oo,t;
		int i=0;
		for(int iy,ix;i<(sizeof(A)+sizeof(B))/2-1&&i<words;i++)
		{
			oo=0;
			ix=i>sizeof(A)/2-1?sizeof(A)/2-1:i;
			iy=i>=sizeof(B)/2-1?i-sizeof(B)/2+1:0;
			for(int j=0;j<=ix-iy;j++)
			{
				t=over;
				over+=anum[ix-j]*bnum[j+i-ix];
				if(over<t)
					oo+=0x10000;
			}
			num[i]=over;
			over>>=16;
			over+=oo;
		}
		while(i<words)
		{
			num[i++]=over;
			over>>=16;
		}
	}
	//multiple& operator *= (multiple &n)
	//{
	//	unsigned short temp[words][words];
	//	unsigned int over;
	//	for(int i=0;i<words;i++)
	//	{
	//		over=0;
	//		for(int j=0;j<words-i;j++)
	//		{
	//			over=num[i]*n.num[j]+over;
	//			temp[i][j]=over;
	//			over>>=16;
	//		}
	//	}
	//	over=0;
	//	for(int i=0;i<words;i++)
	//	{
	//		for(int j=0;j<i+1;j++)
	//		{
	//			over+=temp[i-j][j];
	//		}
	//		num[i]=over;
	//		over>>=16;
	//	}
	//	return *this;
	//}
	bool operator == (int b)
	{
		if((*(int*)(num))!=b)
			return false;
		for(int i=2;i<words;i++)
			if(num[i])
				return false;
		return true;
	}
	bool operator == (multiple &b)
	{
		for(int i=0;i<words;i++)
			if(num[i]!=b.num[i])
				return false;
		return true;
	}
	inline bool operator != (int b)
	{
        return !(*this==b);
	}
	template <typename T>bool operator < (T &b)
	{
		unsigned int *bn=(unsigned int*)(&b);
		int ch;
		if(bits/8>sizeof(T))
		{
			int i=bits/32-sizeof(T)/4;
			while(i--)
				if(dnum[sizeof(T)/4+i-1])
					return false;
			ch=sizeof(T)/4;
		}
		else if(bits/8<sizeof(T))
		{
			int i=sizeof(T)/4-bits/32;
			while(i--)
				if(bn[sizeof(T)/4+i-1])
					return true;
			ch=dwords;
		}
		else
			ch=dwords;
		while(ch--)
		{
			if(dnum[ch]<bn[ch])
				return true;
			else if(dnum[ch]>bn[ch])
				return false;
		}
		return false;
	}
	template <typename T>inline bool operator >(T &b)
	{
		return b<*this;
		}
	template <typename T>inline bool operator <=(T &b)
	{
		return !(b<*this);
	}
	template <typename T>inline bool operator >=(T &b)
	{
		return !(*this<b);
	}
	void LeftShiftWord()
	{
		num[words-1]=num[words-2];
		for(int i=dwords-1;i;i--)
		{
			*(int*)(&num[i*2])=*(int*)(&num[i*2-1]);
		}
		num[1]=num[0];
		num[0]=0;
	}
	void LeftShift()//double
	{
#ifndef USING_ASM
		unsigned int s=0,over=0;
		for(int i=0;i<words/2;i++)
		{
			s=over;
			over=dnum[i]&0x80000000;
			dnum[i]<<=1;
			if(s)
				dnum[i]|=1;
		}
#else
        __asm
		{
			xor eax,eax;
			mov ecx,dwords;
			mov ebx,this;
			lea ebx,[ebx+ecx*4];
			neg ecx;
			xor edx,edx;
			cmp eax,eax;
jnz_back:
			mov eax,dword ptr[ebx+ecx*4];
			add eax,eax;
			jc joo;
			add eax,edx;
			mov dword ptr[ebx+ecx*4],eax;
			xor edx,edx;
			inc ecx;
			jnz jnz_back;
			jmp end;
joo:
			add eax,edx;
			mov dword ptr[ebx+ecx*4],eax;
			mov edx,1;
			inc ecx;
			jnz jnz_back;
end:

		}
#endif
	}
	void RightShift()//half
	{
#ifndef USING_ASM
		unsigned int s=0,under=0;
		for(int i=dwords-1;i>=0;i--)
		{
			s=under;
			under=dnum[i]&1;
			dnum[i]>>=1;
			if(s)
				dnum[i]|=0x80000000;
		}
#else
        __asm
		{
			xor edx,edx;
			mov ecx,dwords;
			mov ebx,this;
			cmp edx,edx;
jnz_back:
			sub ecx,1;
			mov eax,dword ptr[ebx+ecx*4];
			shr eax,1;
			jc jcc;
            add eax,edx;
			mov dword ptr[ebx+ecx*4],eax;
			xor edx,edx;
			test ecx,ecx;
			jnz jnz_back;
			jmp end;
jcc:
            add eax,edx;
			mov dword ptr[ebx+ecx*4],eax;
			mov edx,0x80000000;
			test ecx,ecx;
			jnz jnz_back;
end:
		}
#endif
	}
	multiple& operator <<= (unsigned int shift)
	{
		if(shift>=bits)
		{
			*this=0;
		}
		else
		{
			unsigned short temp[words];
			unsigned short over=0;
			unsigned int sh=shift%16;
			unsigned int ish=16-sh;
			unsigned int ft=shift/16;
			for(int i=0;i<words-ft;i++)
			{
				temp[i+ft]=(num[i]<<sh)|over;
				over=num[i]>>ish;
			}
			while(ft--)
				temp[ft]=0;
			for(int i=0;i<words;i++)
                num[i]=temp[i];
		}
		return *this;
	}
	multiple& operator >>=(unsigned int shift)
	{
		if(shift>=bits)
		{
			*this=0;
		}
		else
		{
			unsigned short temp[words];
			unsigned short under=0;
			int sh=shift%16;
			int ish=16-sh;
			int ft=shift/16;
			for(int i=words-1;i>=ft;i--)
			{
				temp[i-ft]=(num[i]>>sh)|under;
				under=num[i]<<ish;
			}
			while(ft--)
				temp[words-ft-1]=0;
			for(int i=0;i<words;i++)
                num[i]=temp[i];
		}
		return *this;
	}
	template <typename T>void Mod(T &n)
	{
		multiple<bits+32> a=n;
		int shbit=0;
		while(*this>=a)
		{
			a.LeftShiftWord();
			shbit+=16;
		}
		for(;shbit--;)
		{
			a.RightShift();
			if(*this>=a)
			{
				this->Sub(a);
			}
		}
	}
	template<typename T>T operator%(T &m)
	{
		multiple<bits+32> n=m;
		multiple<bits> a=*this;
		if(n==1)
		{
			a=0;
		}
		else if(n==0)
		{
		}
		else
		{
			int shbit=0;
			while(!(a<n))
			{
				n.LeftShiftWord();
				shbit+=16;
			}
			for(;shbit--;)
			{
				n.RightShift();
				if(!(a<n))
				{
					a.Sub(n);
				}
			}
		}
		return a;
	}
	template<typename T>void Div(T &m)
	{
		multiple<bits+32> n=m;
		multiple<bits+32> a=*this;
		if(n==1)
		{
		}
		else if(n==0)
		{
		}
		else
		{
			*this=0;
			int shbit=0;
			while(a>=n)
			{
				n.LeftShiftWord();
				shbit+=16;
			}
			while(shbit--)
			{
				n.RightShift();
				if(a>=n)
				{
					a.Sub(n);
					dnum[shbit/32]|=1<<(shbit%32);
				}
			}
		}
	}
	//multiple& operator /= (multiple &n)
	//{
	//	if(n==0)
	//	{
	//	}
	//	else if(n==1)
	//	{
	//	}
	//	else
	//	{
	//		unsigned short temp[words];
	//		int shbit=0;
	//		while(!(*this<n))
	//		{
	//			n.LeftShift();
	//			shbit++;
	//		}
	//		for(int i=0;i<words;i++)
	//			temp[i]=0;
	//		for(;shbit--;)
	//		{
	//			n.RightShift();
	//			if(!(*this<n))
	//			{
	//				temp[shbit/16]|=1<<(shbit%16);
	//				Sub(n);
	//			}
	//		}
	//		for(int i=0;i<words;i++)
	//			num[i]=temp[i];
	//	}
	//	return *this;
	//}
	template <typename A>void Square(A a)//a*aÅ®this
	{
		unsigned short *anum=(unsigned short *)&a;
		unsigned int ov=0,over=0,oo,t;
		int i=0;
		for(int iy,ix;i<sizeof(A)-1&&i<words;i++)
		{
			oo=0;
			over=0;
			ix=i>sizeof(A)/2-1?sizeof(A)/2-1:i;
			iy=i>=sizeof(A)/2-1?i-sizeof(A)/2+1:0;
			for(int j=0;j<=ix-iy;j++)
			{
				if(ix-j>j+i-ix)
				{
					t=over;
					over+=anum[ix-j]*anum[j+i-ix];
					if(over<t)
						oo+=0x10000;
				}
				else if(ix-j<j+i-ix)
				{
					oo+=oo;
					if(over&0x80000000)
						oo+=0x10000;
					over+=over;
					break;
				}
				else
				{
					oo+=oo;
					if(over&0x80000000)
						oo+=0x10000;
					over+=over;
					t=over;
					over+=anum[ix-j]*anum[j+i-ix];
					if(over<t)
						oo+=0x10000;
					break;
				}
			}
			t=over;
			over+=ov;
			if(over<t)
				oo+=0x10000;
			num[i]=over;
			ov=over>>16;
			ov+=oo;
		}
		while(i<words)
		{
			num[i++]=ov;
			ov>>=16;
		}
	}
	//multiple<bits*2> SpreadSquare()//ìÒèÊÇï‘Ç∑
	//{
	//	multiple<bits*2> temp;
	//	unsigned int over=0,oo,t,of=0;
	//	int i=0;
	//	for(int iy,ix;i<words*2;i++)
	//	{
	//		over=0;
	//		oo=0;
	//		ix=i>=words?words-1:i;
	//		iy=i>=words?i-words+1:0;
	//		for(int j=iy;j<ix+1;j++)
	//		{
	//			if(ix-j+iy==j)
	//			{
	//				oo+=oo;
	//				t=over;
	//				over+=over;
	//				if(over<t)
	//					oo+=0x10000;
	//				t=over;
	//				over+=num[ix-j+iy]*num[j];
	//				if(over<t)
	//					oo+=0x10000;
	//				break;
	//			}
	//			else if(ix-j+iy<j)
	//			{
	//				oo+=oo;
	//				t=over;
	//				over+=over;
	//				if(over<t)
	//					oo+=0x10000;
	//				break;
	//			}
	//			else
	//			{
	//				t=over;
	//				over+=num[ix-j+iy]*num[j];
	//				if(over<t)
	//					oo+=0x10000;
	//			}
	//		}
	//		of+=over;
	//		temp.num[i]=of;
	//		of>>=16;
	//		of+=oo;
	//	}
	//	return temp;
	//}
	//void Square()//ìÒèÊÇ∑ÇÈ
	//{
	//	unsigned __int64 of=0;
	//	unsigned int temp;
	//	unsigned short a[words];
	//	int odd=1;
	//	for(int i=0;i<words;i++)
	//	{
	//		for(int j=0;j<(i+1)/2;j++)
	//		{
	//			temp=num[j]*num[i-j];
	//			if(temp&0x80000000)
	//				of+=0x100000000;
	//			temp+=temp;
	//			of+=temp;
	//		}
	//		if(odd)
	//		{
	//			temp=num[i/2]*num[i/2];
	//			of+=temp;
	//			odd=0;
	//		}
	//		else
	//		{
	//			odd=1;
	//		}
	//		a[i]=(unsigned short)of;
	//		of>>=16;
	//	}
	//	for(int i=0;i<words;i++)
 //           num[i]=a[i];
	//}
	inline bool isodd()
	{
		return num[0]&1;
	}
	multiple& divrem(multiple &n,multiple &div)//div=this/n,this=this%n,èúéZÇ∆ó]ÇËÇèoÇ∑
	{
		int shbit=0;
		while(!(*this<n))
		{
			n.LeftShift();
			shbit++;
		}
		for(int i=0;i<words;i++)
			div.num[i]=0;
		for(;shbit--;)
		{
			n.RightShift();
			if(!(*this<n))
			{
				div.num[shbit/16]|=1<<(shbit%16);
				Sub(n);
			}
		}
		return *this;
	}
	template <typename A,typename B>void GCD(A n,B m)
	{
		B t,u,v;
		t=n%m;
		if(t==0)
		{
			*this=m;
			return;
		}
		u=m%t;
		if(u==0)
		{
			*this=t;
			return;
		}
		for(;;)
		{
			v=t%u;
			if(v==0)
			{
				*this=u;
				break;
			}
            t=u%v;
			if(t==0)
			{
                *this=v;
				break;
			}
            u=v%t;
			if(u==0)
			{
                *this=t;
				break;
			}
		}
	}
	//multiple& gcd(multiple n,multiple m)//nÇ∆mÇÃç≈ëÂåˆñÒêîÇ™ì¸ÇÈ
	//{
	//	multiple t;
	//	for(;;)
	//	{
	//		t=n%m;
	//		if(t==0)
	//			break;
	//		n=m;
	//		m=t;
	//	}
	//	*this=m;
	//	return *this;
	//}
	void powermod(multiple &power,multiple &mod)
	{
		multiple<bits*2> a,t;
		multiple p=power,n=*this,s=1;
        while(p!=0)
		{
			if(p.isodd())
			{
				a.Mul(s,n);
				s=a%mod;
			}
			t.Square(n);
			n=t%mod;
			p.RightShift();
		}
		*this=s;
	}
	void PowerMod(multiple &power,multiple &mod)
	{
		multiple pt[16];
		multiple base;
		multiple<bits*2> a,t;//temp
		multiple p=power,n=*this,s=1,tt;
		//ëOåvéZ
		pt[1]=n;
   		for(int i=2;i<16;i++)
		{
			if(i==2||i==4||i==8)
                t.Square(pt[i/2]);
			else
				t.Mul(pt[i-1],n);
			pt[i]=t%mod;
		}
		t.Square(pt[8]);
		base=t%mod;
		unsigned int bit;
		unsigned int first=1;
		for(int i=bits/4-1;i>=0;i--)
		{
			if(!first)
			{
				t.Square(s);
				s=t%mod;
				t.Square(s);
				s=t%mod;
				t.Square(s);
				s=t%mod;
				t.Square(s);
				s=t%mod;
			}
			bit=(p.dnum[i/8]>>((i%8)*4))&0xf;
			if(bit)
			{
				a.Mul(s,pt[bit]);
				s=a%mod;
				first=0;
			}

		}
		*this=s;
	}
	multiple& modinv(multiple a,multiple n)
	{
		multiple<bits+32> q,s,t,u,v,w,c;
		s=a;
		t=n;
		u=1;
		v=0;
		while(s!=0)
		{
			q=t;
			q.Div(s);
//			std::cout<<"q=";
//			printmp(q);
			w=t;
			c.Mul(q,s);
			w.Sub(c);
//			std::cout<<"w1=";
//			printmp(w);
			t=s;
			s=w;
			w=v;
			c.Mul(q,u);
//			std::cout<<"c=";
//			printmp(c);
			w.Sub(c);
//			std::cout<<"w2=";
//			printmp(w);
			v=u;
			u=w;
//			std::cout<<"u=";
//			printmp(u);
		}
		v.Add(n);
		*this=v%n;
		return *this;
	}
};

template<int bit>class minirsa
{
	multiple<bit> enckey,deckey,mod;
public:
	minirsa()
	{
		enckey=65537;
	}
	void GenerateKey()
	{
		multiple<bit/2> p,q;
		multiple<bit> pq;
		p.GeneratePrime(bit/2);
		q.GeneratePrime(bit/2);
		mod.Mul(p,q);
		p.Dec();
		q.Dec();
		pq.Mul(p,q);
		deckey.modinv(enckey,pq);
	}
	void Export(void *out)
	{
		for(int i=0;i<bit/32;i++)
			((int*)out)[i]=((int*)&mod)[i];
	}
	void Import(void *in)
	{
		for(int i=0;i<bit/32;i++)
			((int*)&mod)[i]=((int*)in)[i];
	}
	void Encode(void *inout)
	{
		multiple<bit> &a=*(multiple<bit>*)inout;
		a.powermod(enckey,mod);
	}
	void Decode(void *inout)
	{
		multiple<bit> &a=*(multiple<bit>*)inout;
		a.powermod(deckey,mod);
	}
};