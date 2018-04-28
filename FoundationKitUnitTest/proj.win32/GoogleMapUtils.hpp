

#include "FoundationKit/Math/Math.hpp"
USING_NS_FK;
namespace GoogleMapping
{
    /// <summary>
    /// Original Author - RALPH BARBAGALLO
    /// https://gist.github.com/flarb/4980598
    /// Source has been extended
    /// </summary>
    class GoogleMapUtils
    {
        static float GOOGLE_OFFSET;
        static float GOOGLE_OFFSET_RADIUS;  //GOOGLEOFFSET / Math::PI;
        static float MATHPI_180;

        static float preLonToX1;
    public:
        static int LonToX(float lon)
        {
            return ((int)Math::Round(GOOGLE_OFFSET + preLonToX1 * lon));
        }

        static int LatToY(float lat)
        {
            return (int)Math::Round(GOOGLE_OFFSET - GOOGLE_OFFSET_RADIUS * Math::Log((1.f + Math::Sin(lat * MATHPI_180)) / (1.f - Math::Sin(lat * MATHPI_180))) / 2.f);
        }

       static float XToLon(float x)
        {
            return ((Math::Round(x) - GOOGLE_OFFSET) / GOOGLE_OFFSET_RADIUS) * 180.f / Math::PI;
        }

        static float YToLat(float y)
        {
            return (Math::PI / 2.f - 2.f * Math::Atan(Math::Exp((Math::Round(y) - GOOGLE_OFFSET) / GOOGLE_OFFSET_RADIUS))) * 180.f / Math::PI;
        }

        static float adjustLonByPixels(float lon, int delta, int zoom)
        {
            return XToLon(LonToX(lon) + (delta << (21 - zoom)));
        }

        static float adjustLatByPixels(float lat, int delta, int zoom)
        {
            return YToLat(LatToY(lat) + (delta << (21 - zoom)));
        }

        static float CalculateScaleX(float lat, int tileSizePixels, int tileSizeUnits, int zoom)
        {
            auto offset = adjustLatByPixels(lat, tileSizePixels, zoom);
            auto y0 = LatToY(lat); auto y1 = LatToY(offset);
            auto rng = y1 - y0;
            return (float)tileSizeUnits / (float)rng;
        }

        static float CalculateScaleY(float lon, int tileSizePixels, int tileSizeUnits, int zoom)
        {
            auto offset = adjustLonByPixels(lon, tileSizePixels, zoom);
            auto x0 = LonToX(lon); auto x1 = LonToX(offset);
            auto rng = x1 - x0;
            return (float)tileSizeUnits / (float)rng;
        }
    };


    float GoogleMapUtils::GOOGLE_OFFSET = 268435456.f;
    float GoogleMapUtils::GOOGLE_OFFSET_RADIUS = 85445659.44705395f;  //GOOGLEOFFSET / Math::PI;
    float GoogleMapUtils::MATHPI_180 = Math::PI / 180.f;
    float GoogleMapUtils::preLonToX1 = GOOGLE_OFFSET_RADIUS * (Math::PI / 180.f);
}

