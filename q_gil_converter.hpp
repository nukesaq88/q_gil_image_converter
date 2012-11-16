/*
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

#ifndef _Q_GIL_CONVERTER_H_
#define _Q_GIL_CONVERTER_H_

#include <limits>
#include <boost/gil/gil_all.hpp>

#include <QImage>


namespace q_gil
{

// convert QRgb to gil pixel
template <typename Pixel_Type>
inline void qrgb_to_gil_pixel( const QRgb &src_rgb, Pixel_Type &dst_pixel )
{
  using namespace boost;
  // pixel concept check
  gil::gil_function_requires< gil::PixelConcept<Pixel_Type> >();

  gil::color_convert(
        gil::rgb8_pixel_t( qRed(src_rgb), qGreen(src_rgb), qBlue(src_rgb) ),
        dst_pixel
        );
}

// convert QRgb(contains alpha channel) to gil pixel
template <typename Pixel_Type>
inline void qrgba_to_gil_pixel( const QRgb &src_rgba, Pixel_Type &dst_pixel )
{
  using namespace boost;
  // pixel concept check
  gil::gil_function_requires< gil::PixelConcept<Pixel_Type> >();

  gil::color_convert(
        gil::rgba8_pixel_t( qRed(src_rgba), qGreen(src_rgba), qBlue(src_rgba), qAlpha(src_rgba) ),
        dst_pixel
        );
}

// convert gil pixel to QRgb(contains alpha channel)
template <typename Pixel_Type>
inline QRgb gil_pixel_to_qrgb( const Pixel_Type &src_pixel )
{
  using namespace boost;
  // pixel concept check
  gil::gil_function_requires< gil::PixelConcept<Pixel_Type> >();

  gil::rgb8_pixel_t rgb_pix;
  gil::color_convert( src_pixel, rgb_pix );
  return qRgb(
        gil::get_color( rgb_pix, gil::red_t() ),
        gil::get_color( rgb_pix, gil::green_t() ),
        gil::get_color( rgb_pix, gil::blue_t() )
        );
}

// convert gil pixel to QRgb
template <typename Pixel_Type>
inline QRgb gil_pixel_to_qrgba( const Pixel_Type &src_pixel )
{
  using namespace boost;
  // pixel concept check
  gil::gil_function_requires< gil::PixelConcept<Pixel_Type> >();

  gil::rgba8_pixel_t rgba_pix;
  gil::color_convert( src_pixel, rgba_pix );
  return qRgba(
        gil::get_color( rgba_pix, gil::red_t() ),
        gil::get_color( rgba_pix, gil::green_t() ),
        gil::get_color( rgba_pix, gil::blue_t() ),
        gil::get_color( rgba_pix, gil::alpha_t() )
        );
}

namespace detail
{

  /*////// convert functors //////*/

  struct qrgb_to_gil_pixel_functor
  {
    template <typename Pixel_Type>
    void operator() ( const QRgb &src_rgb, Pixel_Type &dst_pixel ) const
    { return qrgb_to_gil_pixel( src_rgb, dst_pixel ); }
  };

  struct qrgba_to_gil_pixel_functor
  {
    template <typename Pixel_Type>
    void operator() ( const QRgb &src_rgba, Pixel_Type &dst_pixel ) const
    { return qrgba_to_gil_pixel( src_rgba, dst_pixel ); }
  };

  struct gil_pixel_to_qrgb_functor
  {
    template <typename Pixel_Type>
    QRgb operator() ( const Pixel_Type &src_pixel ) const
    { return gil_pixel_to_qrgb( src_pixel ); }
  };

  struct gil_pixel_to_qrgba_functor
  {
    template <typename Pixel_Type>
    QRgb operator() ( const Pixel_Type &src_pixel ) const
    { return gil_pixel_to_qrgba( src_pixel ); }
  };


  // implementation of qimage_to_gil_image
  template <typename Image_Type, typename Func>
  void qimage_to_gil_image_impl(
    const QImage &src_qimg,
    Image_Type &dst_gil_img,
    Func qrgb_to_pixel_func
    )
  {
    // image concept check
    boost::gil::gil_function_requires< boost::gil::ImageConcept<Image_Type> >();

    typedef typename Image_Type::view_t View_t;
    typedef typename Image_Type::coord_t Coord_t;

    const int w = src_qimg.width();
    const int h = src_qimg.height();

    // gil coordinate type is possible  being less than int
    assert( w < std::numeric_limits<Coord_t>::max() &&
            h < std::numeric_limits<Coord_t>::max() );

    if( dst_gil_img.width() != w || dst_gil_img.height() != h )
      dst_gil_img.recreate( w, h );

    View_t view( boost::gil::view( dst_gil_img ) );
    for( int y = 0; y < h; ++y )
    {
      typename View_t::x_iterator itr = view.row_begin( y );
      for( int x = 0; x < w; ++x, ++itr )
      {
        qrgb_to_pixel_func( src_qimg.pixel( x, y ), *itr );
      }
    }
  }

  // implementation of gil_view_to_qimage
  template <typename View_Type, typename Func>
  QImage gil_view_to_qimage_impl(
    const View_Type &src_view,
    const QImage::Format format,
    Func pixel_to_qrgb_func
    )
  {
    // image view concept check
    boost::gil::gil_function_requires< boost::gil::ImageViewConcept<View_Type> >();

    const typename View_Type::coord_t w = src_view.width();
    const typename View_Type::coord_t h = src_view.height();

    assert( w < std::numeric_limits<int>::max() &&
            h < std::numeric_limits<int>::max() );

    QImage dst_qimg( w, h, format );
    for( int y = 0; y < h; ++y )
    {
      typename View_Type::x_iterator itr = src_view.row_begin( y );
      for( int x = 0; x < w; ++x, ++itr )
      {
        dst_qimg.setPixel( x, y, pixel_to_qrgb_func( *itr ) );
      }
    }

    return dst_qimg;
  }

} // namespace detail

// convert QImage to gil image
// size of dst_gil_img will be truncated same as src_qimg
template <typename Image_Type>
void qimage_to_gil_image( const QImage &src_qimg, Image_Type &dst_gil_img )
{
  // image concept check
  boost::gil::gil_function_requires< boost::gil::ImageConcept<Image_Type> >();

  const bool has_alpha = src_qimg.hasAlphaChannel();
  return has_alpha ?
        detail::qimage_to_gil_image_impl(
          src_qimg, dst_gil_img, detail::qrgba_to_gil_pixel_functor()
          )
      :
        detail::qimage_to_gil_image_impl(
          src_qimg, dst_gil_img, detail::qrgb_to_gil_pixel_functor()
          );
}

// convert gil view to QImage. retrurns created QImage.
template <typename View_Type>
QImage gil_view_to_qimage( const View_Type &src_view )
{
  // image view concept check
  boost::gil::gil_function_requires< boost::gil::ImageViewConcept<View_Type> >();

  const bool has_alpha = boost::gil::contains_color<typename View_Type::value_type,boost::gil::alpha_t>::value;
  return has_alpha ?
        detail::gil_view_to_qimage_impl(
          src_view, QImage::Format_ARGB32, detail::gil_pixel_to_qrgba_functor()
          )
      :
        detail::gil_view_to_qimage_impl(
          src_view, QImage::Format_RGB32, detail::gil_pixel_to_qrgb_functor()
          );
}

} // namespace q_gil

#endif // _Q_GIL_CONVERTER_H_
