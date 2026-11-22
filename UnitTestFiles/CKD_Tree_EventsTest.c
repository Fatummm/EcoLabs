#include "CKD_Tree_Test.h"
#include "IEcoConnectionPointContainer.h"
#include "IEcoEnumConnections.h"
#include "IdKD_Tree.h"
#include "IKD_TreeEvents.h"

#define TEST_NODES 7
#define TEST_VISITS 3
#define TEST_PRUNES 2

typedef struct {
    uint16_t nodes;
    uint16_t visits;
    uint16_t prunes;
} KDTreeEventCount;

typedef struct {
    IKD_TreeEventsVTbl* pVTbl;
    uint32_t refs;
    IEcoStdIOC89* out;
    IKD_Tree** sources;
    KDTreeEventCount counts[2];
    uint16_t step;
    int16_t failed;
    uint16_t id;
} CKD_TreeSink;

static int16_t ECOCALLMETHOD CKD_TreeSink_QueryInterface(IKD_TreeEvents* me, const UGUID* riid, voidptr_t* ppv) {
    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    *ppv = 0;
    if (!IsEqualUGUID(riid, &IID_IKD_TreeEvents) && !IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        return ERR_ECO_NOINTERFACE;
    }
    *ppv = me;
    me->pVTbl->AddRef(me);
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CKD_TreeSink_AddRef(IKD_TreeEvents* me) {
    return ++((CKD_TreeSink*)me)->refs;
}

/* The test owns both receivers on the stack until all subscriptions are removed. */
static uint32_t ECOCALLMETHOD CKD_TreeSink_Release(IKD_TreeEvents* me) {
    return --((CKD_TreeSink*)me)->refs;
}

static int16_t CKD_TreeSink_Source(CKD_TreeSink* sink, IKD_Tree* source) {
    if (source == sink->sources[0]) {
        return 0;
    }
    if (source == sink->sources[1]) {
        return 1;
    }
    sink->failed = 1;
    return -1;
}

static int16_t ECOCALLMETHOD CKD_TreeSink_OnBuildNode(IKD_TreeEvents* me, IKD_Tree* source,
    uint16_t axis, int32_t left, int32_t median, int32_t right)
{
    CKD_TreeSink* sink = (CKD_TreeSink*)me;
    int16_t index = CKD_TreeSink_Source(sink, source);

    if (index < 0 || axis >= 2 || left > median || median >= right) {
        sink->failed = 1;
        return ERR_ECO_FAIL;
    }
    sink->counts[index].nodes++;
    sink->out->pVTbl->printf(sink->out,
        "  Sink %d / Tree %c / Step %d: build [%d, %d), median %d, axis %d\n",
        sink->id, 'A' + index, ++sink->step, (int)left, (int)right, (int)median, axis);
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeSink_OnSearchVisit(IKD_TreeEvents* me, IKD_Tree* source,
    uint16_t axis, double squaredDistance, uint16_t candidateCount)
{
    CKD_TreeSink* sink = (CKD_TreeSink*)me;
    int16_t index = CKD_TreeSink_Source(sink, source);

    if (index < 0 || axis >= 2 || squaredDistance < 0 || candidateCount != 1) {
        sink->failed = 1;
        return ERR_ECO_FAIL;
    }
    sink->counts[index].visits++;
    sink->out->pVTbl->printf(sink->out,
        "  Sink %d / Tree %c / Step %d: visit, axis %d, distance^2 %.0f, candidates %d\n",
        sink->id, 'A' + index, ++sink->step, axis, squaredDistance, candidateCount);
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CKD_TreeSink_OnSearchPrune(IKD_TreeEvents* me, IKD_Tree* source,
    uint16_t axis, double squaredPlaneDistance, double squaredWorstDistance)
{
    CKD_TreeSink* sink = (CKD_TreeSink*)me;
    int16_t index = CKD_TreeSink_Source(sink, source);

    if (index < 0 || axis >= 2 || squaredPlaneDistance < squaredWorstDistance) {
        sink->failed = 1;
        return ERR_ECO_FAIL;
    }
    sink->counts[index].prunes++;
    sink->out->pVTbl->printf(sink->out,
        "  Sink %d / Tree %c / Step %d: prune, axis %d, plane^2 %.0f, worst^2 %.0f\n",
        sink->id, 'A' + index, ++sink->step, axis, squaredPlaneDistance, squaredWorstDistance);
    return ERR_ECO_SUCCESS;
}

static IKD_TreeEventsVTbl g_xKDTreeSinkVTbl = {
    CKD_TreeSink_QueryInterface,
    CKD_TreeSink_AddRef,
    CKD_TreeSink_Release,
    CKD_TreeSink_OnBuildNode,
    CKD_TreeSink_OnSearchVisit,
    CKD_TreeSink_OnSearchPrune
};

static int16_t CKD_Tree_Test_FindPoint(IKD_Tree* tree, IEcoConnectionPoint** point) {
    IEcoConnectionPointContainer* container = 0;
    IEcoConnectionPointContainer* owner = 0;
    IEcoEnumConnectionPoints* list = 0;
    IEcoConnectionPoint* listed = 0;
    UGUID iid;
    int16_t result;

    result = tree->pVTbl->QueryInterface(tree, &IID_IEcoConnectionPointContainer, (voidptr_t*)&container);
    if (result != ERR_ECO_SUCCESS || container == 0) {
        return ERR_ECO_NOINTERFACE;
    }
    result = ERR_ECO_FAIL;
    if (container->pVTbl->FindConnectionPoint(container, &IID_IKD_TreeEvents, point) == ERR_ECO_SUCCESS &&
        (*point)->pVTbl->GetConnectionInterface(*point, &iid) == ERR_ECO_SUCCESS &&
        IsEqualUGUID(&iid, &IID_IKD_TreeEvents) &&
        (*point)->pVTbl->GetConnectionPointContainer(*point, &owner) == ERR_ECO_SUCCESS && owner == container &&
        container->pVTbl->EnumConnectionPoints(container, &list) == ERR_ECO_SUCCESS &&
        list->pVTbl->Next(list, 1, &listed, 0) == ERR_ECO_SUCCESS && listed == *point &&
        list->pVTbl->Skip(list, 1) != ERR_ECO_SUCCESS) {
        result = ERR_ECO_SUCCESS;
    }
    if (listed != 0) {
        listed->pVTbl->Release(listed);
    }
    if (list != 0) {
        list->pVTbl->Release(list);
    }
    if (owner != 0) {
        owner->pVTbl->Release(owner);
    }
    container->pVTbl->Release(container);
    return result;
}

static int16_t CKD_Tree_Test_EventRun(IKD_Tree* tree, IEcoMemoryAllocator1* mem) {
    int16_t coords[TEST_NODES][2] = {
        {-6, 0}, {-4, 0}, {-2, 0}, {0, 0}, {2, 0}, {4, 0}, {6, 0}
    };
    KDPoint values[TEST_NODES];
    KDPointPtr_t points[TEST_NODES];
    KDSearchResult found = {0, 0, 0};
    int16_t result;
    int16_t i;

    for (i = 0; i < TEST_NODES; i++) {
        values[i].coords = coords[i];
        points[i] = &values[i];
    }
    result = tree->pVTbl->BuildTree(tree, points, TEST_NODES, 2);
    if (result == ERR_ECO_SUCCESS) {
        result = tree->pVTbl->SearchKNearest(tree, &values[TEST_NODES - 1], 1, &found);
    }
    if (result == ERR_ECO_SUCCESS &&
        (found.count != 1 || found.distances[0] != 0 ||
         ((int16_t*)found.points[0]->coords)[0] != 6 ||
         ((int16_t*)found.points[0]->coords)[1] != 0)) {
        result = ERR_ECO_FAIL;
    }
    if (found.points != 0) {
        mem->pVTbl->Free(mem, found.points);
    }
    if (found.distances != 0) {
        mem->pVTbl->Free(mem, found.distances);
    }
    return result;
}

static int16_t CKD_Tree_Test_EventScenario(CKD_TreeSink* sinks, IEcoMemoryAllocator1* mem,
    char_t* name, uint16_t sourceCount, const uint16_t expected[2][2])
{
    IEcoStdIOC89* out = sinks[0].out;
    KDTreeEventCount* count;
    uint16_t source;
    uint16_t sink;
    uint16_t connected;
    int16_t result = ERR_ECO_SUCCESS;

    out->pVTbl->printf(out, "\n[EVENTS] %s\n", name);
    for (source = 0; source < sourceCount; source++) {
        if (CKD_Tree_Test_EventRun(sinks[0].sources[source], mem) != ERR_ECO_SUCCESS) {
            result = ERR_ECO_FAIL;
        }
    }
    for (sink = 0; sink < 2; sink++) {
        for (source = 0; source < 2; source++) {
            count = &sinks[sink].counts[source];
            connected = expected[sink][source];
            if (sinks[sink].failed || count->nodes != TEST_NODES * connected ||
                count->visits != TEST_VISITS * connected || count->prunes != TEST_PRUNES * connected) {
                result = ERR_ECO_FAIL;
            }
            count->nodes = count->visits = count->prunes = 0;
        }
        sinks[sink].step = 0;
    }
    out->pVTbl->printf(out, "  %s\n", result == ERR_ECO_SUCCESS ? "PASS" : "FAIL");
    return result;
}

int16_t CKD_Tree_Test_Events(IEcoInterfaceBus1* pIBus, IEcoMemoryAllocator1* pIMem, IEcoStdIOC89* pIStdIO) {
    static const uint16_t oneToMany[2][2] = {{1, 0}, {1, 0}};
    static const uint16_t manyToOne[2][2] = {{1, 1}, {0, 0}};
    static const uint16_t manyToMany[2][2] = {{1, 1}, {1, 1}};
    static const uint16_t disconnected[2][2] = {{0, 1}, {1, 0}};
    CKD_TreeSink sinks[2] = {{0}, {0}};
    IKD_Tree* trees[2] = {0, 0};
    IEcoConnectionPoint* points[2] = {0, 0};
    uint32_t cookies[2][2] = {{0, 0}, {0, 0}};
    uint32_t oldCookie;
    uint16_t source;
    uint16_t sink;
    int16_t result = ERR_ECO_FAIL;

    for (source = 0; source < 2; source++) {
        if (pIBus->pVTbl->QueryComponent(pIBus, &CID_KD_Tree, 0, &IID_IKD_Tree,
                (voidptr_t*)&trees[source]) != ERR_ECO_SUCCESS ||
            trees[source] == 0 ||
            CKD_Tree_Test_FindPoint(trees[source], &points[source]) != ERR_ECO_SUCCESS) {
            goto Release;
        }
        sinks[source].pVTbl = &g_xKDTreeSinkVTbl;
        sinks[source].refs = 1;
        sinks[source].out = pIStdIO;
        sinks[source].sources = trees;
        sinks[source].id = source + 1;
    }
    if (trees[0] == trees[1] || points[0] == points[1]) {
        goto Release;
    }

    for (sink = 0; sink < 2; sink++) {
        if (points[0]->pVTbl->Advise(points[0], (IEcoUnknown*)&sinks[sink], &cookies[0][sink]) != ERR_ECO_SUCCESS ||
            cookies[0][sink] == 0) {
            goto Release;
        }
    }
    if (cookies[0][0] == cookies[0][1] ||
        CKD_Tree_Test_EventScenario(sinks, pIMem, "One source -> two sinks", 1, oneToMany) != ERR_ECO_SUCCESS) {
        goto Release;
    }

    oldCookie = cookies[0][1];
    if (points[0]->pVTbl->Unadvise(points[0], oldCookie) != ERR_ECO_SUCCESS) {
        goto Release;
    }
    cookies[0][1] = 0;
    if (points[0]->pVTbl->Unadvise(points[0], oldCookie) == ERR_ECO_SUCCESS ||
        points[1]->pVTbl->Advise(points[1], (IEcoUnknown*)&sinks[0], &cookies[1][0]) != ERR_ECO_SUCCESS ||
        CKD_Tree_Test_EventScenario(sinks, pIMem, "Two sources -> one sink", 2, manyToOne) != ERR_ECO_SUCCESS) {
        goto Release;
    }

    if (points[0]->pVTbl->Advise(points[0], (IEcoUnknown*)&sinks[1], &cookies[0][1]) != ERR_ECO_SUCCESS ||
        cookies[0][1] == oldCookie ||
        points[1]->pVTbl->Advise(points[1], (IEcoUnknown*)&sinks[1], &cookies[1][1]) != ERR_ECO_SUCCESS ||
        CKD_Tree_Test_EventScenario(sinks, pIMem, "Two sources -> two sinks", 2, manyToMany) != ERR_ECO_SUCCESS) {
        goto Release;
    }

    for (source = 0; source < 2; source++) {
        if (points[source]->pVTbl->Unadvise(points[source], cookies[source][source]) != ERR_ECO_SUCCESS) {
            goto Release;
        }
        cookies[source][source] = 0;
    }
    result = CKD_Tree_Test_EventScenario(sinks, pIMem, "Independent Unadvise", 2, disconnected);

Release:
    for (source = 0; source < 2; source++) {
        if (points[source] != 0) {
            for (sink = 0; sink < 2; sink++) {
                if (cookies[source][sink] != 0 &&
                    points[source]->pVTbl->Unadvise(points[source], cookies[source][sink]) != ERR_ECO_SUCCESS) {
                    result = ERR_ECO_FAIL;
                }
            }
            points[source]->pVTbl->Release(points[source]);
        }
        if (trees[source] != 0) {
            trees[source]->pVTbl->Release(trees[source]);
        }
    }
    if (sinks[0].refs != 1 || sinks[1].refs != 1) {
        result = ERR_ECO_FAIL;
    }
    return result;
}
