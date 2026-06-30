#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/FLAlertLayer.hpp>

static float setting_rotation_offset = Mod::get()->getSettingValue<float>("rotation-offset");
static float setting_rotation_time = Mod::get()->getSettingValue<float>("rotation-time");
static std::string setting_rotation_easing = Mod::get()->getSettingValue<std::string>("rotation-easing");
static std::string setting_rotation_easing_type = Mod::get()->getSettingValue<std::string>("rotation-easing-type");

static float setting_scale_time = Mod::get()->getSettingValue<float>("scale-time");
static std::string setting_scale_easing = Mod::get()->getSettingValue<std::string>("scale-easing");
static std::string setting_scale_easing_type = Mod::get()->getSettingValue<std::string>("scale-easing-type");

static float setting_skew_offset_x = Mod::get()->getSettingValue<float>("skew-offset-x");
static float setting_skew_offset_y = Mod::get()->getSettingValue<float>("skew-offset-y");
static float setting_skew_time = Mod::get()->getSettingValue<float>("skew-time");
static std::string setting_skew_easing = Mod::get()->getSettingValue<std::string>("skew-easing");
static std::string setting_skew_easing_type = Mod::get()->getSettingValue<std::string>("skew-easing-type");

static bool ignore_elasticity = Mod::get()->getSettingValue<bool>("ignore-elasticity");

$on_mod(Loaded) {
    listenForSettingChanges<float>(
        "rotation-offset",
        [](float value) { setting_rotation_offset = value; });

    listenForSettingChanges<float>(
        "rotation-time",
        [](float value) { setting_rotation_time = value; });

    listenForSettingChanges<std::string>(
        "rotation-easing",
        [](std::string value) { setting_rotation_easing = value; });

    listenForSettingChanges<std::string>(
        "rotation-easing-type",
        [](std::string value) { setting_rotation_easing_type = value; });

    listenForSettingChanges<float>(
        "scale-time",
        [](float value) { setting_scale_time = value; });

    listenForSettingChanges<std::string>(
        "scale-easing",
        [](std::string value) { setting_scale_easing = value; });

    listenForSettingChanges<std::string>(
        "scale-easing-type",
        [](std::string value) { setting_scale_easing_type = value; });

    listenForSettingChanges<float>(
        "skew-offset-x",
        [](float value) { setting_skew_offset_x = value; });

    listenForSettingChanges<float>(
        "skew-offset-y",
        [](float value) { setting_skew_offset_y = value; });

    listenForSettingChanges<float>(
        "skew-time",
        [](float value) { setting_skew_time = value; });

    listenForSettingChanges<std::string>(
        "skew-easing",
        [](std::string value) { setting_skew_easing = value; });

    listenForSettingChanges<std::string>(
        "skew-easing-type",
        [](std::string value) { setting_skew_easing_type = value; });

    listenForSettingChanges<bool>(
        "ignore-elasticity",
        [](bool value) { ignore_elasticity = value; });

};

CCAction* easeActionFromString(CCActionInterval* uneased, std::string easing, std::string easingType);

// There is a visual issue where running the Back easing with InOut/In shows the popup at negative scale for a second
// This function reverts any Back easing with InOut/In as it's easing type to just use the  Out easingType
CCAction* scaleEaseActionFromString(CCActionInterval* uneased, std::string easing, std::string easingType) {
	if (easing == "Back") {
		if (easingType == "In" || easingType == "In Out") {
			return CCEaseBackOut::create(uneased);
		}
	}
	return easeActionFromString(uneased, easing, easingType);
}

CCAction* easeActionFromString(CCActionInterval* uneased, std::string easing, std::string easingType) {
	if (easingType == "In") {
		if (easing == "Back") {
			return CCEaseBackIn::create(uneased);
		}
		if (easing == "Elastic") {
			return CCEaseElasticIn::create(uneased);
		}
		if (easing == "Ease") {
			return CCEaseIn::create(uneased, 2.0f);
		}
		if (easing == "Exponential") {
			return CCEaseExponentialIn::create(uneased);
		}
		if (easing == "Sine") {
			return CCEaseSineIn::create(uneased);
		}
		if (easing == "Bounce") {
			return CCEaseBounceIn::create(uneased);
		}
	}
	if (easingType == "Out") {
		if (easing == "Back") {
			return CCEaseBackOut::create(uneased);
		}
		if (easing == "Elastic") {
			return CCEaseElasticOut::create(uneased, 0.60000002f);
		}
		if (easing == "Ease") {
			return CCEaseOut::create(uneased, 2.0f);
		}
		if (easing == "Exponential") {
			return CCEaseExponentialOut::create(uneased);
		}
		if (easing == "Sine") {
			return CCEaseSineOut::create(uneased);
		}
		if (easing == "Bounce") {
			return CCEaseBounceOut::create(uneased);
		}
	}
	
	// Either something stupid happened or easingType is InOut. Either way, I don't give a shit, I'm doing InOut.
	if (easingType == "In Out") {
		if (easing == "Back") {
			return CCEaseBackInOut::create(uneased);
		}
		if (easing == "Elastic") {
			return CCEaseElasticInOut::create(uneased);
		}
		if (easing == "Ease") {
			return CCEaseInOut::create(uneased, 2.0f);
		}
		if (easing == "Exponential") {
			return CCEaseExponentialInOut::create(uneased);
		}
		if (easing == "Sine") {
			return CCEaseSineInOut::create(uneased);
		}
		if (easing == "Bounce") {
			return CCEaseBounceInOut::create(uneased);
		}
	}

	return uneased;
}


class $modify(FLAlertLayer) {
	void show() {

		float opacity = this->getOpacity();

		FLAlertLayer::show();

		// Fix incompat with chroma-icons
		if (this->getID() == "naxrin.chroma_icons/chroma-icons-central") {
			return;
		}

		this->stopAllActions();

		if (!this->m_noElasticity || ignore_elasticity) {
			this->m_mainLayer->setScale(0);
			this->m_mainLayer->setSkewX(setting_skew_offset_x);
			this->m_mainLayer->setSkewY(setting_skew_offset_y);
			this->m_mainLayer->setRotation(-setting_rotation_offset);

			auto rotateAction = CCRotateTo::create(setting_rotation_time, 0.0);
			auto skewAction = CCSkewTo::create(setting_skew_time, 0.0, 0.0);
			auto scaleAction = CCScaleTo::create(setting_scale_time, 1.0);
			
			auto easedScaleAction = scaleEaseActionFromString(scaleAction, setting_scale_easing, setting_scale_easing_type);
			auto easedSkewAction = easeActionFromString(skewAction, setting_skew_easing, setting_rotation_easing_type);
			auto easedRotationAction = easeActionFromString(rotateAction, setting_rotation_easing, setting_rotation_easing_type);

			auto fadeAction = CCFadeTo::create(0.14, opacity); // Fade in the FLAlertLayer
			this->runAction(fadeAction);
			this->m_mainLayer->runAction(easedScaleAction);
			this->m_mainLayer->runAction(easedSkewAction);
			this->m_mainLayer->runAction(easedRotationAction);
		} else {
			
		}
	}
};