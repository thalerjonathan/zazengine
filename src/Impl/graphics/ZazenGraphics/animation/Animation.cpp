#include "Animation.h"

#include "../ZazenGraphics.h"

using namespace std;

Animation::Animation( double durationTicks, double ticksPerSecond )
{
	this->m_skeletonRoot = NULL;

	this->m_currentTime = 0.0;
	this->m_currentFrame = 0.0;
	
	this->m_lastPosChannelIndex = 0;
	this->m_lastRotChannelIndex = 0;
	this->m_lastScaleChannelIndex = 0;

	this->m_ticksPerSecond = ticksPerSecond;
	this->m_durationTicks = durationTicks;
	this->m_durationInSec = this->m_durationTicks / this->m_ticksPerSecond;
}

Animation::~Animation()
{
	// TODO: cleanup recursively
}

void
Animation::update()
{
	double timeAdvance = ZazenGraphics::getInstance().getCore().getProcessingFactor();

	// not initialized with a skeleton yet
	if ( NULL == this->m_skeletonRoot )
	{
		return;
	}

	this->m_transforms.clear();

	this->m_currentTime = fmod( ( this->m_currentTime + timeAdvance ), this->m_durationInSec );
	this->m_currentFrame = this->m_currentTime * this->m_ticksPerSecond;

	this->animateSkeleton( this->m_skeletonRoot, glm::mat4( ) );
}

void
Animation::initSkeleton( MeshNode* rootMeshNode )
{
	if ( NULL == this->m_skeletonRoot && rootMeshNode )
	{
		this->m_skeletonRoot = this->buildAnimationSkeleton( rootMeshNode, glm::mat4() );
	}
}

Animation::AnimationSkeletonPart*
Animation::buildAnimationSkeleton( MeshNode* rootMeshNode, const glm::mat4& parentGlobalTransform )
{
	AnimationSkeletonPart* skeletonPart = new AnimationSkeletonPart();
	skeletonPart->m_name = rootMeshNode->getName();
	skeletonPart->m_localTransform = rootMeshNode->getModelMatrix();
	skeletonPart->m_globalTransform = parentGlobalTransform * skeletonPart->m_localTransform;

	std::map<std::string, AnimationNode*>::iterator findAnimationNodeIter = this->m_animationNodes.find( rootMeshNode->getName() );
	std::map<std::string, AnimationBone*>::iterator findAnimationBoneIter = this->m_animationBones.find( rootMeshNode->getName() );

	if ( this->m_animationNodes.end() != findAnimationNodeIter )
	{
		skeletonPart->m_animationNode = findAnimationNodeIter->second;

		skeletonPart->m_localTransform = skeletonPart->m_animationNode->m_transform;
		skeletonPart->m_globalTransform = parentGlobalTransform * skeletonPart->m_localTransform;
	}

	if ( this->m_animationBones.end() != findAnimationBoneIter && rootMeshNode->getBone() )
	{
		skeletonPart->m_animationBone = findAnimationBoneIter->second;
		// replace bone-offset with the one from rootMeshNode if present
		//skeletonPart->m_animationBone->m_offset = rootMeshNode->getBone()->m_offset;
	}

	const std::vector<MeshNode*>& children = rootMeshNode->getChildren();
	for ( unsigned int i = 0; i < children.size(); i++ )
	{
		AnimationSkeletonPart* childSkeletonPart = this->buildAnimationSkeleton( children[ i ], skeletonPart->m_globalTransform );
		skeletonPart->m_children.push_back( childSkeletonPart );
	}

	return skeletonPart;
}

void
Animation::animateSkeleton( AnimationSkeletonPart* skeletonPart, const glm::mat4& parentGlobalTransform )
{
	glm::mat4 localNodeTransform;

	// check if this node has animation-information stored
	if ( skeletonPart->m_animationNode )
	{
		AnimationNode* animationNode = skeletonPart->m_animationNode;
		AnimationChannel& channel = animationNode->m_animationChannels;

		glm::mat4 translationMatrix;
		glm::mat4 rotationMatrix;
		glm::mat4 scalingMatrix;

		this->interpolatePosition( channel.m_positionKeys, translationMatrix );
		this->interpolateRotation( channel.m_rotationKeys, rotationMatrix );
		this->interpolateScaling( channel.m_scalingKeys, translationMatrix );

		// this animation-transformation replaces the local transformation
		// order is important: first scaling, then rotation and last translation (matrix-multiplication applies the operation first which one was multiplied with last)
		localNodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}
	else
	{
		// no animation for this node: use local transformation 
		localNodeTransform = skeletonPart->m_localTransform;
	}

	// do hierarchical transformation: apply parentTransformation to local Transformation
	glm::mat4 globalTransform = parentGlobalTransform * localNodeTransform;

	// check if this node has a bone assosiated with
	if ( skeletonPart->m_animationBone )
	{
		const AnimationBone* bone = skeletonPart->m_animationBone;
		// calculate bone-transformation: we need to apply the mesh-to-bone transformation 
		glm::mat4 globalBoneTransform = localNodeTransform * bone->m_offset;

		this->m_transforms.push_back( globalTransform );
	}

	const std::vector<AnimationSkeletonPart*>& skeletonChildren = skeletonPart->m_children;

	for ( unsigned int i = 0; i < skeletonChildren.size(); i++ )
	{
		this->animateSkeleton( skeletonChildren[ i ], globalTransform );
	}
}

void
Animation::interpolatePosition( const vector<AnimationKey<glm::vec3>>& positionKeys, glm::mat4& translationMatrix )
{
	// no translations, leave untouched
	if ( 0 == positionKeys.size() )
	{
		return;
	}

	// exactly one translation, nothing to interpolate
	if ( positionKeys.size() == 1 )
	{
		translationMatrix[ 3 ][ 0 ] = positionKeys[ 0 ].m_value.x;
		translationMatrix[ 3 ][ 1 ] = positionKeys[ 0 ].m_value.y;
		translationMatrix[ 3 ][ 2 ] = positionKeys[ 0 ].m_value.z;

		return;
	}

	AnimationKey<glm::vec3> firstPos;
	AnimationKey<glm::vec3> nextPos;

	this->findFirstAndNext<glm::vec3>( positionKeys, firstPos, nextPos, this->m_lastPosChannelIndex );

	translationMatrix[ 3 ][ 0 ] = firstPos.m_value.x;
	translationMatrix[ 3 ][ 1 ] = firstPos.m_value.y;
	translationMatrix[ 3 ][ 2 ] = firstPos.m_value.z;

	/*
	float normalizedTime = ( float ) ( ( this->m_currentFrame - firstPos.m_time ) / ( nextPos.m_time - firstPos.m_time ) );

	glm::vec3 interpolatedPos = ( ( 1.0f - normalizedTime ) * firstPos.m_value ) + ( normalizedTime * nextPos.m_value );
	translationMatrix[ 3 ][ 0 ] = interpolatedPos.x;
	translationMatrix[ 3 ][ 1 ] = interpolatedPos.y;
	translationMatrix[ 3 ][ 2 ] = interpolatedPos.z;
	*/
}

void
Animation::interpolateRotation( const std::vector<AnimationKey<glm::quat>>& rotationKeys, glm::mat4& rotationMatrix )
{
	// no rotations, leave untouched
	if ( 0 == rotationKeys.size() )
	{
		return;
	}

	// exactly one rotation, nothing to interpolate
	if ( rotationKeys.size() == 1 )
	{
		rotationMatrix = glm::mat4_cast( rotationKeys[ 0 ].m_value );

		return;
	}

	AnimationKey<glm::quat> firstRot;
	AnimationKey<glm::quat> nextRot;

	this->findFirstAndNext<glm::quat>( rotationKeys, firstRot, nextRot, this->m_lastRotChannelIndex );

	// for now: now interpolation, just take first found
	rotationMatrix = glm::mat4_cast( firstRot.m_value );

	// TODO: do interpolation of scaling between first and next
}

void
Animation::interpolateScaling( const std::vector<AnimationKey<glm::vec3>>& scalingKeys, glm::mat4& scalingMatrix )
{
	// no scalings, leave untouched
	if ( 0 == scalingKeys.size() )
	{
		return;
	}

	// exactly one translation, nothing to interpolate
	if ( scalingKeys.size() == 1 )
	{
		scalingMatrix[ 0 ][ 0 ] = scalingKeys[ 0 ].m_value.x;
		scalingMatrix[ 1 ][ 1 ] = scalingKeys[ 0 ].m_value.y;
		scalingMatrix[ 2 ][ 2 ] = scalingKeys[ 0 ].m_value.z;

		return;
	}

	AnimationKey<glm::vec3> firstScaling;
	AnimationKey<glm::vec3> nextScaling;
	
	this->findFirstAndNext<glm::vec3>( scalingKeys, firstScaling, nextScaling, this->m_lastScaleChannelIndex );

	// for now: now interpolation, just take first found
	scalingMatrix[ 0 ][ 0 ] = firstScaling.m_value.x;
	scalingMatrix[ 1 ][ 1 ] = firstScaling.m_value.y;
	scalingMatrix[ 2 ][ 2 ] = firstScaling.m_value.z;

	// TODO: do interpolation of scaling between first and next
}

template <typename T>
void
Animation::findFirstAndNext( const vector<AnimationKey<T>>& keys, AnimationKey<T>& first, AnimationKey<T>& next, unsigned int& lastChannelIndex )
{
	// jump in time or frame turned around => reset last index to 0
	if ( ( unsigned int ) this->m_currentFrame < lastChannelIndex - 1 )
	{
		lastChannelIndex = 0;
	}

	// NOTE: if the time would be equally spaced by 1 unit then m_time would match the index and we could access it in O(1)
	for ( unsigned int i = lastChannelIndex; i < keys.size(); i++ )
	{
		unsigned int nextIndex = ( i + 1 ) % keys.size();

		if ( keys[ i ].m_time >= this->m_currentFrame )
		{
			first = keys[ i ];
			next = keys[ nextIndex ];

			lastChannelIndex = i;
			return;
		}
	}
}
