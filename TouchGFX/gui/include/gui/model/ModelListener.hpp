/*
 *****************************************************************************
 * @attention
 *
 * Portion Copyright (C) 2024 Semilla3 OÜ.  All Rights Reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
	ModelListener() : model(0) {}

	virtual ~ModelListener() {}

	void bind(Model* m)
	{
		model = m;
	}

	virtual void updateVolume(uint16_t state) {}
	virtual void updateFingerprint(uint16_t state) {}

protected:
	Model* model;
};

#endif // MODELLISTENER_HPP
