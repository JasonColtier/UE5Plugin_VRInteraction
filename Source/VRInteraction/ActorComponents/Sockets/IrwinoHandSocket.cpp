// Fill out your copyright notice in the Description page of Project Settings.

#include "IrwinoHandSocket.h"

#include <Components/PoseableMeshComponent.h>

UIrwinoHandSocket::UIrwinoHandSocket(const FObjectInitializer& Initializer): Super(Initializer)
{
	OffsetFlipAxis   = EAxis::Y;
	OffsetMirrorAxis = EAxis::X;
	bWantsFlipAxis   = false;
	CustomOffset     = FTransform::Identity;
}

bool UIrwinoHandSocket::GetBlendedPoseSnapShot(FPoseSnapshot& PoseSnapShot, UPoseableMeshComponent* TargetMesh, bool bSkipRootBone, bool bFlipHand)
{
	if (HandTargetAnimation) // && bUseCustomPoseDeltas && CustomPoseDeltas.Num() > 0)
	{
		PoseSnapShot.SkeletalMeshName = HandTargetAnimation->GetSkeleton()->GetFName();
		PoseSnapShot.SnapshotName     = HandTargetAnimation->GetFName();
		PoseSnapShot.BoneNames.Empty();
		PoseSnapShot.LocalTransforms.Empty();
		TArray<FName> OrigBoneNames;

		if (USkeleton* AnimationSkele = HandTargetAnimation->GetSkeleton())
		{
			// pre-size the array to avoid unnecessary reallocation
			PoseSnapShot.BoneNames.AddUninitialized(AnimationSkele->GetReferenceSkeleton().GetNum());
			OrigBoneNames.AddUninitialized(AnimationSkele->GetReferenceSkeleton().GetNum());
			for (int32 i = 0; i < AnimationSkele->GetReferenceSkeleton().GetNum(); i++)
			{
				PoseSnapShot.BoneNames[i] = AnimationSkele->GetReferenceSkeleton().GetBoneName(i);
				OrigBoneNames[i]          = PoseSnapShot.BoneNames[i];
				if (bFlipHand)
				{
					FString bName = PoseSnapShot.BoneNames[i].ToString();

					if (bName.Contains("_r"))
					{
						bName = bName.Replace(TEXT("_r"), TEXT("_l"));
					}
					else
					{
						bName = bName.Replace(TEXT("_l"), TEXT("_r"));
					}

					PoseSnapShot.BoneNames[i] = FName(bName);
				}
			}
		}
		else
		{
			return false;
		}

		const FReferenceSkeleton& RefSkeleton = (TargetMesh) ? TargetMesh->GetSkinnedAsset()->GetRefSkeleton() : HandTargetAnimation->GetSkeleton()->GetReferenceSkeleton();
		FTransform                LocalTransform;

		const TArray<FTrackToSkeletonMap>& TrackMap   = HandTargetAnimation->GetCompressedTrackToSkeletonMapTable();
		int32                              TrackIndex = INDEX_NONE;

		for (int32 BoneNameIndex = 0; BoneNameIndex < PoseSnapShot.BoneNames.Num(); ++BoneNameIndex)
		{
			TrackIndex = INDEX_NONE;
			if (BoneNameIndex < TrackMap.Num() && TrackMap[BoneNameIndex].BoneTreeIndex == BoneNameIndex)
			{
				TrackIndex = BoneNameIndex;
			}
			else
			{
				// This shouldn't happen but I need a fallback
				// Don't currently want to reconstruct the map inversely
				for (int i = 0; i < TrackMap.Num(); ++i)
				{
					if (TrackMap[i].BoneTreeIndex == BoneNameIndex)
					{
						TrackIndex = i;
						break;
					}
				}
			}

			const FName& BoneName = PoseSnapShot.BoneNames[BoneNameIndex];

			if (TrackIndex != INDEX_NONE && (!bSkipRootBone || TrackIndex != 0))
			{
				HandTargetAnimation->GetBoneTransform(LocalTransform, TrackIndex, 0.f, false);
			}
			else
			{
				// otherwise, get ref pose if exists
				const int32 BoneIDX = RefSkeleton.FindBoneIndex(BoneName);
				if (BoneIDX != INDEX_NONE)
				{
					LocalTransform = RefSkeleton.GetRefBonePose()[BoneIDX];
				}
				else
				{
					LocalTransform = FTransform::Identity;
				}
			}

			if (bUseCustomPoseDeltas)
			{
				FQuat DeltaQuat = FQuat::Identity;
				if (FBPVRHandPoseBonePair* HandPair = CustomPoseDeltas.FindByKey(OrigBoneNames[BoneNameIndex]))
				{
					DeltaQuat = HandPair->DeltaPose;
				}

				LocalTransform.ConcatenateRotation(DeltaQuat);
				LocalTransform.NormalizeRotation();
			}

			if (bFlipHand && (!bSkipRootBone || TrackIndex != 0))
			{
				FMatrix M = LocalTransform.ToMatrixWithScale();
				M.Mirror(EAxis::X, EAxis::X);
				M.Mirror(EAxis::Y, EAxis::Y);
				M.Mirror(EAxis::Z, EAxis::Z);
				LocalTransform.SetFromMatrix(M);
			}

			PoseSnapShot.LocalTransforms.Add(LocalTransform);
		}

		PoseSnapShot.bIsValid = true;
		return true;
	}
	else if (bUseCustomPoseDeltas && CustomPoseDeltas.Num() && TargetMesh)
	{
		PoseSnapShot.SkeletalMeshName = TargetMesh->GetSkinnedAsset()->GetSkeleton()->GetFName();
		PoseSnapShot.SnapshotName     = FName(TEXT("RawDeltaPose"));
		PoseSnapShot.BoneNames.Empty();
		PoseSnapShot.LocalTransforms.Empty();
		TargetMesh->GetBoneNames(PoseSnapShot.BoneNames);

		PoseSnapShot.LocalTransforms = TargetMesh->GetSkinnedAsset()->GetSkeleton()->GetRefLocalPoses();

		FQuat DeltaQuat      = FQuat::Identity;
		FName TargetBoneName = NAME_None;

		for (FBPVRHandPoseBonePair& HandPair : CustomPoseDeltas)
		{
			if (bFlipHand)
			{
				FString bName = HandPair.BoneName.ToString();

				if (bName.Contains("_r"))
				{
					bName = bName.Replace(TEXT("_r"), TEXT("_l"));
				}
				else
				{
					bName = bName.Replace(TEXT("_l"), TEXT("_r"));
				}

				TargetBoneName = FName(bName);
			}
			else
			{
				TargetBoneName = HandPair.BoneName;
			}

			int32 BoneIdx = TargetMesh->GetBoneIndex(TargetBoneName);
			if (BoneIdx != INDEX_NONE)
			{
				DeltaQuat = HandPair.DeltaPose;

				if (bFlipHand)
				{
					FTransform DeltaTrans(DeltaQuat);
					FMatrix    M = DeltaTrans.ToMatrixWithScale();
					M.Mirror(EAxis::X, EAxis::X);
					M.Mirror(EAxis::Y, EAxis::Y);
					M.Mirror(EAxis::Z, EAxis::Z);
					DeltaTrans.SetFromMatrix(M);
					DeltaQuat = DeltaTrans.GetRotation();
				}

				PoseSnapShot.LocalTransforms[BoneIdx].ConcatenateRotation(DeltaQuat);
				PoseSnapShot.LocalTransforms[BoneIdx].NormalizeRotation();
			}
		}

		PoseSnapShot.bIsValid = true;
		return true;
	}

	return false;
}
