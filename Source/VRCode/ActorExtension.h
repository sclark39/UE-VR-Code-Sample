#pragma once

#include "GameFramework/Actor.h"

class AActorExt
{
public:
	UActorComponent* AActorExt::GetComponentByName(const ANSICHAR* name) const
	{
		FName fname(name, FNAME_Find);
		if (fname == NAME_None)
			return NULL;

		const TSet<UActorComponent*> &components = reinterpret_cast<const AActor*>(this)->GetComponents();
		for (UActorComponent* component : components)
			if (component->GetFName() == fname)
				return component;

		return NULL;
	}
};