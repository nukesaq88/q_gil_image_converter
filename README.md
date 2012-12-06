q_gil_image_converter
=====================

Convert QImage to Boost.GIL image, and convert Boost.GIL imageview to QImage.
This library is header-only.

License
-------
 Use, modification and distribution are subject to the Boost Software License,
 Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 http://www.boost.org/LICENSE_1_0.txt).

 Example
 -------

 Example of converting QImage to Boost.GIL image.
 ```cpp
 const QImage qimage( "foo.png" );

 boost::gil::rgb8_image_t gil_rgb8_image;
 q_gil::qimage_to_gil_image( qimage, gil_rgb8_image );

 gil::bgr32f_image_t gil_bgr32f_image;
 boost::q_gil::qimage_to_gil_image( qimage, gil_bgr32f_image );
 ```

 Example of converting Boost.GIL view to QImage.
 ```cpp
 boost::gil::rgba8_image_t gil_rgba8_image;
 boost::gil::png_read_image( "bar.png", gil_rgba8_image );

 const QImage qimage = q_gil::gil_view_to_qimage( boost::gil::const_view( gil_rgba8_image ) );
 ```
