#ifndef OCRENDERER_H_
#define OCRENDERER_H_

#include <list>

#include "Renderer.h"

class OCRenderer : public Renderer
{
 public:
	OCRenderer(Camera& camera, std::string&);
	virtual ~OCRenderer();

	 // renders this list of geominstances which must be in front-to-back order
	 void renderFrame(GeomInstance*);

 private:
	 bool parentIntersectingFrustum;

	  typedef struct {
		 GLuint id;
		 std::list<GeomInstance*> items;	// used for multiqueries: when size > 1 => multiqueries, otherwise single-query
	 } QueryItem;

	 std::list<GeomInstance*> distanceQueue;

	 std::list<GeomInstance*> iQueries;
	 std::list<GeomInstance*> vQueries;

	 std::list<QueryItem> queries;

	 bool firstQueryFinished();
	 void traverseInstance(GeomInstance*);
	 void handleReturnedQuery(QueryItem);
	 void pullUpVisibility(GeomInstance*);
	 void queryIndividualNodes(QueryItem);
	 void issueQuery(GeomInstance*);
	 void issueMultiQueries();
	 void issuePreviouslyInvisibleNode(GeomInstance*);
};

#endif /*OCRENDERER_H_*/
