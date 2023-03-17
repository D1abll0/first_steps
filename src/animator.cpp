#include "../include/animator.h"

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(100);

	for(int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(mat4(1.0f));
}

void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	if(m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

//mat4 Animator::GetTransMat(float dt, string name, vec3& p, quat& r)
//{
//	mat4 nodeTransform = mat4(1.0);
//
//	Bone* Bone = m_CurrentAnimation->FindBone(name);
//
//	if(Bone)
//	{
//		Bone->Update(m_CurrentTime);
//		nodeTransform = Bone->GetLocalTransform();
//		p = Bone->m_Positions[4].position;
//		r = Bone->m_Rotations[4].orientation;
//	}
//	return nodeTransform;
//}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, mat4 parentTransform)
{
	string nodeName = node->name;
	mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if(Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();		
	}

	mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if(boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

vector<mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}