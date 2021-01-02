#ifndef _ILLUMINANT_EXPORTER_H
#define _ILLUMINANT_EXPORTER_H

#include <string>
#include <vector>

#include "filianore/core/elemental.h"

class IlluminantExporter
{
public:
    IlluminantExporter()
    {
    }

    std::vector<std::shared_ptr<filianore::Illuminant>> ExportIlluminants();
};

#endif