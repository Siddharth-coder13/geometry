#ifndef BOOST_GEOMETRY_GEOMETRIES_POLYHEDRON_HPP
#define BOOST_GEOMETRY_GEOMETRIES_POLYHEDRON_HPP

#include <memory>
#include <vector>

#include <boost/concept/assert.hpp>

#include <boost/geometry/geometries/concepts/point_concept.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/tags.hpp>

#include <boost/config.hpp>
#ifndef BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#include <initializer_list>
#endif

namespace boost { namespace geometry 
{

namespace model
{


template 
<
    typename Point,
    bool ClockWise = true, bool Closed = true,
    template<typename, typename> class PointList = std::vector,
    template<typename, typename> class RingList = std::vector,
    template<typename> class PointAlloc = std::allocator,
    template<typename> class RingAlloc = std::allocator
>
class polyhedron : public PointList<ring<Point>, RingAlloc<ring<Point> > >
{
	BOOST_CONCEPT_ASSERT( (concepts::Point<Point>) );

public :

	typedef Point point_type;
    typedef ring<Point, ClockWise, Closed, PointList, PointAlloc> ring_type;
    typedef RingList<ring_type, RingAlloc<ring_type > > polyhedron_type;
    //typedef RingList<Point, RingAlloc<Point> > base_type;

#ifndef BOOST_NO_CXX11_HDR_INITIALIZER_LIST

    /// \constructor_default{polyhedron}
    inline polyhedron()
	    : polyhedron_type()
	{}

    /// \constructor_initialized_list{polyhedron}
	inline polyhedron(std::initializer_list<ring_type> l)
	    : polyhedron_type(l.begin(), l.end())
	{}

#endif
	
};

} // namespace model

#ifndef DOXYGEN_NO_TRAITS_SPECIALIZATIONS
namespace traits
{

template
<
    typename Point,
    bool ClockWise, bool Closed,
    template<typename, typename> class PointList,
    template<typename, typename> class RingList,
    template<typename> class PointAlloc,
    template<typename> class RingAlloc
>
struct tag
<
    model::polyhedron
        <
            Point, 
            ClockWise, Closed,
            PointList, RingList, 
            PointAlloc, RingAlloc
        > 
>
{
	typedef polyhedron_tag type;
};


} // namespace traits
#endif // DOXYGEN_NO_TRAITS_SPECIALIZATIONS

}} // namespace boost::geometry

#endif