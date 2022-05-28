#pragma once

#include "Vendor.h"

class QPixmap;
class QImage;
class QColor;
class QSize;
class QPointF;
class QRectF;

namespace cute {

/// This file contains a bunch of utility functions that make it easier to use
/// some Qt features.
/// @author Abdullah Aghazadah
/// @date 10-11-16
/// @file

namespace QtUtils {

QPixmap pixmap_from_color(QSize size, QColor color);
double distance(QPointF p1, QPointF p2);

} // namespace QtUtils

} // namespace cute
