#include "util.h"

#include <maya/MSelectionList.h>

MStatus GetDependencyNodeByName(const MString &name, MObject &node) {
    MSelectionList selList;
    selList.add(name);

    if (selList.isEmpty()) {
        return MS::kFailure;
    }

    return selList.getDependNode(0, node);
}