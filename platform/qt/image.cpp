#include <mbgl/util/image.hpp>
#include <mbgl/platform/log.hpp>

#include <QImage>

namespace mbgl {
namespace util {

std::string compress_png(int, int, const void *) {
    Log::Error(Event::Image, "Function compress_png not implemented");
    return std::string();
}

Image::Image(std::string const& data) {
    QImage image =
        QImage::fromData(reinterpret_cast<const uchar*>(data.data()), data.size()).rgbSwapped();

    if (image.isNull()) {
        Log::Error(Event::Image, "Error loading image");
        return;
    }

    image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    width = image.width();
    height = image.height();

    img = std::make_unique<char[]>(image.byteCount());
    memcpy(img.get(), image.constBits(), image.byteCount());
}

}
}
