#ifndef FIVEBYFIVEMATRIX_HH
#define FIVEBYFIVEMATRIX_HH

#include <vector>

#include <ostream>
#include <array>

#include "Vector5.hh"
#include "Vector3D.hh"

namespace aidaTT
{

  // typedef Eigen::Matrix<double, 5, 1 > Vector5d ;
  // typedef Eigen::Matrix<double, 5, 5, Eigen::RowMajor > Matrix5x5d;

  /** this is the main working horse class for a 5x5 matrix
   *  Implemented using an std::array.
   * 
   *  @author F.Gaede, DESY
   *  @date Sep 2016
   */

  class fiveByFiveMatrix {

    friend class IPropagation;

  public:
    /** the default construction, it initializes all entries to zero **/
    fiveByFiveMatrix() : _m{{ {{0.,0.,0.,0.,0.}},  {{0.,0.,0.,0.,0.}} , {{0.,0.,0.,0.,0.}} , {{0.,0.,0.,0.,0.}} , {{0.,0.,0.,0.,0.}} }} {}

    /** copy construction **/
    fiveByFiveMatrix(const fiveByFiveMatrix& o) : _m(o._m) {} 

    /** the construction with a vector, this is ROW wise;
     * the sixth element of the vector is the first in the second row!
     **/
    fiveByFiveMatrix(const std::vector<double>& o)  {
      
      std::memcpy( &_m[0][0] , &o[0] , 25*sizeof(double) ) ;
    }

    /** the destructor **/
    ~fiveByFiveMatrix() {}

    /** the assignment operator **/
    fiveByFiveMatrix& operator=(const fiveByFiveMatrix&o ){
      if( this != &o ){ _m = o._m ; }
      return *this ;
    }

    /** direct read access to the individual matrix elements by index**/
    double operator()(unsigned int r, unsigned int c) const{
      return _m[r][c] ;
    }

    /** direct write access to the individual matrix elements by index**/
    double& operator()(unsigned int r, unsigned int c){
      return _m[r][c] ;
    }
    
    /** define matrix-matrix multipliation **/
    fiveByFiveMatrix operator*(const fiveByFiveMatrix& o){
      fiveByFiveMatrix p ;
      for( unsigned i=0 ; i<5 ; ++i )
	for( unsigned j=0 ; j<5 ; ++j )
	  for( unsigned k=0 ; k<5 ; ++k )
	    p._m[i][j] += _m[i][k] * o._m[k][i] ;
      // unroll this loop ?
      return p ;
    }
    
    /** define matrix-vector multiplication **/
    Vector5 operator*(const Vector5& o) const {
      Vector5 v ;
      for( unsigned j=0 ; j<5 ; ++j )
	for( unsigned k=0 ; k<5 ; ++k )
	  v(j) += _m[j][k] * o(k) ;
      // unroll this loop ?
      return v ;
    }
    
    /** make a unit matrix out of the given matrix **/
    inline void Unit(){
      _m = {{ {{1.,0.,0.,0.,0.}},  {{0.,1.,0.,0.,0.}} , {{0.,0.,1.,0.,0.}} , {{0.,0.,0.,1.,0.}} , {{0.,0.,0.,0.,1.}} }} ;
    }
      
    /** transpose the matrix (in place) */
    inline void Transpose(){
      for( unsigned i=0 ; i<5 ; ++i )
	for( unsigned j=i+1 ; j<5 ; ++j )
	  std::swap(  _m[i][j] , _m[j][i]   ) ;
    }
    
    /** get array to construct other matrix representations */
    double* array() const{
      return (double*) &_m[0][0];
    }
    
        
  private:
    std::array<std::array<double, 5>, 5> _m ;
  };



  inline std::ostream & operator << (std::ostream & os, const fiveByFiveMatrix& m)
  {
    os << "[" << m(0, 0) << " , " << m(0, 1) << " , " << m(0, 2) << " , " << m(0, 3) << " , " << m(0, 4) << " | " << std::endl
       << " " << m(1, 0) << " , " << m(1, 1) << " , " << m(1, 2) << " , " << m(1, 3) << " , " << m(1, 4) << " | " << std::endl
       << " " << m(2, 0) << " , " << m(2, 1) << " , " << m(2, 2) << " , " << m(2, 3) << " , " << m(2, 4) << " | " << std::endl
       << " " << m(3, 0) << " , " << m(3, 1) << " , " << m(3, 2) << " , " << m(3, 3) << " , " << m(3, 4) << " | " << std::endl
       << " " << m(4, 0) << " , " << m(4, 1) << " , " << m(4, 2) << " , " << m(4, 3) << " , " << m(4, 4) << " ]"  << std::endl;
    return os ;
  }

  typedef fiveByFiveMatrix fiveByFiveMatrixSym;
  typedef fiveByFiveMatrixSym fullCovariance;
}

#endif // FIVEBYFIVEMATRIX_HH
