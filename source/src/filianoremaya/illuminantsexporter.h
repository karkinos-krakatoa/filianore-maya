#ifndef _ILLUMINANT_EXPORTER_H
#define _ILLUMINANT_EXPORTER_H

#include <string>
#include <vector>

#include "filianore/core/elemental.h"

struct IlluminantExporterResponse {
    std::vector<std::shared_ptr<filianore::Illuminant>> illuminants;
    std::vector<std::shared_ptr<filianore::Primitive>> prims;
};

class IlluminantExporter {
public:
    IlluminantExporter() {
    }

    IlluminantExporterResponse ExportIlluminants(const std::vector<std::shared_ptr<filianore::Primitive>> &prims);
};

#endif