#ifndef SINGLETON_H
#define SINGLETON_H


/*------------------------------------------------
						����ģʽ����
-------------------------------------------------*/

template <class T>
class CSingleton
{
public:

	static T* GetSingletonPtr()
	{
		static T instance;
		return &instance;
	}

	inline static T& GetSingleton() { return *GetSingletonPtr(); }


protected:
	CSingleton(){}
	virtual ~CSingleton(){}
	//��ֹ���ÿ������캯���͸�ֵ���캯��
	CSingleton(const CSingleton&);
	CSingleton& operator= (const CSingleton&);
};


#define DECLEAR_SINGLETON(classname) friend class CSingleton<classname>; 


#endif
