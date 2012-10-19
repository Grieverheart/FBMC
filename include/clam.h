#ifndef CLAM_H
#define CLAM_H

namespace clam{
	//////////////////////////Vector//////////////////////////
	class vec3d{
	protected:
		double data_[3];
	public:
		vec3d(void){};
		vec3d(double x, double y, double z);
		~vec3d(void){};
		vec3d cross(vec3d const &other)const;
		friend double dot(vec3d const &a, vec3d const &b);
		friend double length(vec3d const &a);
		double operator[](unsigned int)const;
		double& operator[](unsigned int);
	};
	double dot(vec3d const &a, vec3d const &b);
	double length(vec3d const &a);
	
	class vec3f{
	protected:
		float data_[3];
	public:
		vec3f(void){};
		vec3f(float x, float y, float z);
		~vec3f(void){};
		vec3f cross(vec3f const &other)const;
		friend float dot(vec3f const &a, vec3f const &b);
		friend float length(vec3f const &a);
		float operator[](unsigned int)const;
		float& operator[](unsigned int);
	};
	float dot(vec3f const &a, vec3f const &b);
	float length(vec3f const &a);

	class vec3i{
	protected:
		int data_[3];
	public:
		vec3i(void){};
		vec3i(int x, int y, int z);
		~vec3i(void){};
		vec3i cross(vec3i const &other)const;
		friend int dot(vec3i const &a, vec3i const &b);
		friend int length(vec3i const &a);
		int operator[](unsigned int)const;
		int& operator[](unsigned int);
	};
	int dot(vec3i const &a, vec3i const &b);
	int length(vec3i const &a);
	
	//////////////////////////Matrix//////////////////////////
	class mat3d{
	protected:
		double data_[9];
	public:
		mat3d(void){for(int i = 0; i < 9; i++) data_[i] = 0.0;};
		mat3d(double const x);
		mat3d(vec3d const &x, vec3d const &y, vec3d const &z);
		~mat3d(void){};
		vec3d Col(unsigned int)const;
		friend mat3d operator*(mat3d const &a, mat3d const &b);
		double operator[](unsigned int)const;
		double& operator[](unsigned int);
	};
	
	class mat3f{
	protected:
		float data_[9];
	public:
		mat3f(void){for(int i = 0; i < 9; i++) data_[i] = 0.0;};
		mat3f(float const x);
		mat3f(vec3f const &x, vec3f const &y, vec3f const &z);
		~mat3f(void){};
		vec3f Col(unsigned int)const;
		friend mat3f operator*(mat3f const &a, mat3f const &b);
		float operator[](unsigned int)const;
		float& operator[](unsigned int);
	};
}

#endif