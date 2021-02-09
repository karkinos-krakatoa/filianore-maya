#ifndef _ILLUMINANT_EXPORTER_H
#define _ILLUMINANT_EXPORTER_H

#include <string>
#include <vector>

#include "filianore/core/elemental.h"

using namespace filianore;

class IlluminantExporter
{
public:
    IlluminantExporter()
    {
    }

    std::vector<std::shared_ptr<filianore::Illuminant>> ExportIlluminants(std::vector<std::shared_ptr<Primitive>> *prims);
};

#endif