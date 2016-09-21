#ifndef VECTOR5_HH
#define VECTOR5_HH

#include <ostream>
#include <vector>
#include <array>


namespace aidaTT
{


  /** This is a simple abstraction of a five-element vector 
   *  based on an array.  
   *  Considerably faster than the implementation using an Eigen matrix...
   * 
   *  @author F.Gaede, DESY
   *  @date Sep 2016
   */
  class Vector5 {

    friend class fiveByFiveMatrix;

  public:
    /** the default construction, it initializes all entries to zero **/
    Vector5() : _v{{0.,0.,0.,0.,0.}} {}  

    /** copy constructor **/
    Vector5(const Vector5& o) : _v( o._v ) {}

    /** the construction with a vector
     **/
    Vector5(const std::vector<double>& o) {
      std::memcpy( &_v[0] , &o[0] , 5*sizeof(double) ) ;
    }

    /** the construction with an array
     **/
    Vector5(const double* o) {
      std::memcpy( &_v[0] , o , 5*sizeof(double) ) ;
    }

    /** the construction from five doubles
     **/
    Vector5(double v0, double v1, double v2, double v3, double v4) :
      _v { { v0,v1,v2,v3,v4 } } {
    }

    /** the destructor **/
    ~Vector5(){}

    /** assignment operator **/
    Vector5& operator=(const Vector5& o){
      if( this != &o ) {
	_v = o._v ;
      }
      return *this ;
    }

    // addition operator  - in place !?
    Vector5& operator+=(const Vector5& o){ 
      _v[0] += o._v[0] ;
      _v[1] += o._v[1] ;
      _v[2] += o._v[2] ;
      _v[3] += o._v[3] ;
      _v[4] += o._v[4] ;
      return *this ;
    }

    // addition w/ copy
    Vector5 operator+(const Vector5& o) const{
      Vector5 newVec( *this ) ;
      newVec += o ;
      return  newVec ;
    } 


    /** Direct read access to the individual elements by index**/
    double operator()(unsigned int i) const{
      return _v[i] ;
    }

    /** direct write access to the individual elements by index**/
    double& operator()(unsigned int i){
      return _v[i] ;
    }

  protected:

  private:
    std::array<double,5> _v ;
  };


  inline std::ostream & operator << (std::ostream & os, const Vector5& v)
  {
    os << " {" << v(0) << " , " << v(1) << " , " << v(2) << " , " << v(3) << " , " << v(4)  << "} "  ;
    return os ;
  }
}
#endif // VECTOR5_HH
