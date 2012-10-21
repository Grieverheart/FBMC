#ifndef CLAM_H
#define CLAM_H

#include <math.h>

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
		double operator[](unsigned int i)const{
			return data_[i];
		}
		double& operator[](unsigned int i){
			return data_[i];
		}
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
		float operator[](unsigned int i)const{
			return data_[i];
		}
		float& operator[](unsigned int i){
			return data_[i];
		}
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
		int operator[](unsigned int i)const{
			return data_[i];
		}
		int& operator[](unsigned int i){
			return data_[i];
		}
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
		vec3d Row(unsigned int)const;
		friend mat3d operator*(mat3d const &a, mat3d const &b);
		double operator[](unsigned int i)const{
			return data_[i];
		}
		double& operator[](unsigned int i){
			return data_[i];
		}
		mat3d inverse(void)const;
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
		vec3f Row(unsigned int)const;
		friend mat3f operator*(mat3f const &a, mat3f const &b);
		float operator[](unsigned int i)const{
			return data_[i];
		}
		float& operator[](unsigned int i){
			return data_[i];
		}
	};
	
	
	////////////////////////Function Definitions//////////////////////////////////
	////////////////////////////////Vec3f/////////////////////////////////////////
	inline vec3d::vec3d(double x, double y, double z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	inline vec3d vec3d::cross(vec3d const &other_v)const{
		vec3d cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	inline double dot(vec3d const &a, vec3d const &b){
		double result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	inline double length(vec3d const &a){
		return sqrt(dot(a, a));
	}
	
	////////////////////////////////Vec3d//////////////////////////////////////////
	inline vec3f::vec3f(float x, float y, float z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	inline vec3f vec3f::cross(vec3f const &other_v)const{
		vec3f cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	inline float dot(vec3f const &a, vec3f const &b){
		float result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	inline float length(vec3f const &a){
		return sqrt(dot(a, a));
	}
	
	////////////////////////////////Vec3i//////////////////////////////////////////
	inline vec3i::vec3i(int x, int y, int z){
		data_[0] = x;
		data_[1] = y;
		data_[2] = z;
	}

	inline vec3i vec3i::cross(vec3i const &other_v)const{
		vec3i this_v = *this;
		vec3i cross_p;
		cross_p[0] = data_[1] * other_v[2] - data_[2] * other_v[1];
		cross_p[1] = data_[2] * other_v[0] - data_[0] * other_v[2];
		cross_p[2] = data_[0] * other_v[1] - data_[1] * other_v[0];
		return cross_p;
	}

	inline int dot(vec3i const &a, vec3i const &b){
		int result = 0.0;
		for(int i = 0; i < 3; i++) result += a[i]*b[i];
		return result;
	}
	
	inline int length(vec3i const &a){
		return int(sqrt(dot(a, a)));
	}
	
	////////////////////////////////Mat3d//////////////////////////////////////////
	inline mat3d::mat3d(double const x){
		for(int i = 0; i < 9; i++) data_[i] = 0.0;
		for(int i = 0; i < 3; i++) data_[3*i+i] = x;
	}
	
	inline mat3d::mat3d(vec3d const &x, vec3d const &y, vec3d const &z){
		data_[0] = x[0];
		data_[1] = y[0];
		data_[2] = z[0];
		data_[3] = x[1];
		data_[4] = y[1];
		data_[5] = z[1];
		data_[6] = x[2];
		data_[7] = y[2];
		data_[8] = z[2];
	}

	inline vec3d mat3d::Row(unsigned int i)const{
		return vec3d(data_[3*i+0], data_[3*i+1], data_[3*i+2]);
	}

	inline vec3d mat3d::Col(unsigned int i)const{
		return vec3d(data_[i], data_[3+i], data_[6+i]);
	}
	
	inline mat3d operator*(mat3d const &a, mat3d const &b){
		mat3d result;
		
		for(int i = 0; i < 3; i++){
			for(int k = 0; k < 3; k++){
				for(int j = 0; j < 3; j++){
					result[3*k+i] += a[3*j+i] * b[3*k+j];   
				}
			}
		}
		
		return result;
	}
	
	inline mat3d mat3d::inverse(void)const{
		double determinant =
			(*this)[0] * ((*this)[4] * (*this)[8] - (*this)[5] * (*this)[7]) - 
			(*this)[1] * ((*this)[8] * (*this)[3] - (*this)[5] * (*this)[6]) + 
			(*this)[2] * ((*this)[3] * (*this)[7] - (*this)[4] * (*this)[6]);
		
		determinant = 1.0 / determinant;
		
		mat3d inverse;
		
		inverse[0] = determinant * ((*this)[4] * (*this)[8] - (*this)[5] * (*this)[7]);
		inverse[1] = determinant * ((*this)[2] * (*this)[7] - (*this)[1] * (*this)[8]);
		inverse[2] = determinant * ((*this)[1] * (*this)[5] - (*this)[2] * (*this)[4]);
		inverse[3] = determinant * ((*this)[5] * (*this)[6] - (*this)[8] * (*this)[3]);
		inverse[4] = determinant * ((*this)[0] * (*this)[8] - (*this)[2] * (*this)[6]);
		inverse[5] = determinant * ((*this)[2] * (*this)[3] - (*this)[0] * (*this)[5]);
		inverse[6] = determinant * ((*this)[3] * (*this)[7] - (*this)[4] * (*this)[6]);
		inverse[7] = determinant * ((*this)[6] * (*this)[1] - (*this)[0] * (*this)[7]);
		inverse[8] = determinant * ((*this)[0] * (*this)[4] - (*this)[1] * (*this)[3]);
		
		return inverse;
	}
	
	////////////////////////////////Mat3f//////////////////////////////////////////
	inline mat3f::mat3f(float const x){
		for(int i = 0; i < 9; i++) data_[i] = 0.0;
		for(int i = 0; i < 3; i++) data_[3*i+i] = x;
	}
	
	inline mat3f::mat3f(vec3f const &x, vec3f const &y, vec3f const &z){
		data_[0] = x[0];
		data_[1] = y[0];
		data_[2] = z[0];
		data_[3] = x[1];
		data_[4] = y[1];
		data_[5] = z[1];
		data_[6] = x[2];
		data_[7] = y[2];
		data_[8] = z[2];
	}

	inline vec3f mat3f::Row(unsigned int i)const{
		return vec3f(data_[3*i+0], data_[3*i+1], data_[3*i+2]);
	}

	inline vec3f mat3f::Col(unsigned int i)const{
		return vec3f(data_[i], data_[3+i], data_[6+i]);
	}
	
	inline mat3f operator*(mat3f const &a, mat3f const &b){
		mat3f result;
		
		for(int i = 0; i < 3; i++){
			for(int k = 0; k < 3; k++){
				for(int j = 0; j < 3; j++){
					result[3*k+i] += a[3*j+i] * b[3*k+j];   
				}
			}
		}
		
		return result;
	}
}

#endif