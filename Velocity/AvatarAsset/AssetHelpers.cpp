#include "AssetHelpers.h"

QString AssetHelpers::AssetSubcategoryToString(AssetSubcategory category)
{
	switch (category)
	{
		case CarryableCarryable:
            return QString("Carryable, Carryable");
		case CostumeCasualSuit:
            return QString("Costume, Casual Suit");
		case CostumeCostume:
            return QString("Costume, Costume");
		case CostumeFormalSuit:
            return QString("Costume, Formal Suit");
		case CostumeLast:
            return QString("Costume, Last");
		case CostumeLongDress:
            return QString("Costume, Long Dress");
		case CostumeShortDress:
            return QString("Costume, Short Dress");
		case EarringsDanglers:
            return QString("Earrings, Danglers");
		case EarringsLargehoops:
            return QString("Earrings, Largehoops");
		case EarringsSingleDangler:
            return QString("Earrings, Single Dangler");
		case EarringsSingleLargeHoop:
            return QString("Earrings, Single Large Hoop");
		case EarringsSingleSmallHoop:
            return QString("Earrings, Single Small Hoop");
		case EarringsSingleStud:
            return QString("Earrings, Single Stud");
		case EarringsSmallHoops:
            return QString("Earrings, Small Hoops");
		case EarringsStuds:
            return QString("Earrings, Studs");
		case GlassesCostume:
            return QString("Glasses, Costume");
		case GlassesGlasses:
            return QString("Glasses, Glasses");
		case GlassesSunglasses:
            return QString("Glasses, Sunglasses");
		case GlovesFingerless:
            return QString("Gloves, Fingerless");
		case GlovesFullFingered:
            return QString("Gloves, Full Fingered");
		case HatBaseballCap:
            return QString("Hat, Baseball Cap");
		case HatBeanie:
            return QString("Hat, Beanie");
		case HatBearskin:
            return QString("Hat, Bearskin");
		case HatBrimmed:
            return QString("Hat, Brimmed");
		case HatCostume:
            return QString("Hat, Costume");
		case HatFez:
            return QString("Hat, Fez");
		case HatFlatCap:
            return QString("Hat, Flat Cap");
		case HatHeadwrap:
            return QString("Hat, Headwrap");
		case HatHelmet:
            return QString("Hat, Helmet");
		case HatPeakCap:
            return QString("Hat, Peak Cap");
		case RingLast:
            return QString("Ring, Last");
		case RingLeft:
            return QString("Ring, Left");
		case RingRight:
            return QString("Ring, Right");
		case ShirtCoat:
            return QString("Shirt, Coat");
		case ShirtHoodie:
            return QString("Shirt, Hoodie");
		case ShirtJacket:
            return QString("Shirt, Jacket");
		case ShirtLongSleeveShirt:
            return QString("Shirt, Long Sleeve Shirt");
		case ShirtLongSleeveTee:
            return QString("Shirt, Long Sleeve Tee");
		case ShirtPolo:
            return QString("Shirt, Polo");
		case ShirtShortSleeveShirt:
            return QString("Shirt, Short Sleeve Shirt");
		case ShirtSportsTee:
            return QString("Shirt, Sports Tee");
		case ShirtSweater:
            return QString("Shirt, Sweater");
		case ShirtTee:
            return QString("Shirt, Tee");
		case ShirtVest:
            return QString("Shirt, Vest");
		case ShoesCostume:
            return QString("Shoes, Costume");
		case ShoesFormal:
            return QString("Shoes, Formal");
		case ShoesHeels:
            return QString("Shoes, Heels");
		case ShoesHighBoots:
            return QString("Shoes, High Boots");
		case ShoesPumps:
            return QString("Shoes, Pumps");
		case ShoesSandals:
            return QString("Shoes, Sandals");
		case ShoesShortBoots:
            return QString("Shoes, Short Boots");
		case ShoesTrainers:
            return QString("Shoes, Trainers");
		case TrousersCargo:
            return QString("Trousers, Cargo");
		case TrousersHotpants:
            return QString("Trousers, Hotpants");
		case TrousersJeans:
            return QString("Trousers, Jeans");
		case TrousersKilt:
            return QString("Trousers, Kilt");
		case TrousersLeggings:
            return QString("Trousers, Leggings");
		case TrousersLongShorts:
            return QString("Trousers, Long Shorts");
		case TrousersLongSkirt:
            return QString("Trousers, Long Skirt");
		case TrousersShorts:
            return QString("Trousers, Shorts");
		case TrousersShortSkirt:
            return QString("Trousers, Short Skirt");
		case TrousersTrousers:
            return QString("Trousers, Trousers");
		case WristwearBands:
            return QString("Wristwear, Bands");
		case WristwearBracelet:
            return QString("Wristwear, Bracelet");
		case WristwearSweatbands:
            return QString("Wristwear, Sweatbands");
		case WristwearWatch:
            return QString("Wristwear, Watch");
		default:
            throw QString("Asset: Error converting 'AssetSubcategory' to string, invalid value.\n");
	}
}

QString AssetHelpers::AssetGenderToString(AssetGender gender)
{
	switch (gender)
	{
		case Male:
            return QString("Male");
		case Female:
            return QString("Female");
		case Both:
            return QString("Both");
		default:
            throw QString("Asset: Error converting 'AssetGender' to string, invalid value.");
	}
}

QString AssetHelpers::SkeletonVersionToString(SkeletonVersion version)
{
	switch (version)
	{
		case Nxe:
            return QString("Nxe");
		case Natal:
            return QString("Natal");
		default:
            throw QString("Asset: Invalid 'Skeleton Version' value.\n");
	}
}

QString AssetHelpers::BlockIDToString(STRRBBlockId id)
{
	switch (id)
	{
		case STRBAnimation:
            return QString("Animation");
		case STRBAssetMetadata:
            return QString("Asset Metadata");
		case STRBAssetMetadataVersioned:
            return QString("Asset Metadata Versioned");
		case STRBCustomColorTable:
            return QString("Custom Color Table");
		case STRBEof:
            return QString("Eof");
		case STRBInvalid:
            return QString("Invalid");
		case STRBModel:
            return QString("Model");
		case STRBShapeOverrides:
            return QString("Shape Overrides");
		case STRBSkeleton:
            return QString("Skeleton");
		case STRBTexture:
            return QString("Texture");
		default:
            throw QString("STRB: Invalid 'STRBBlockId' value.\n");
	}
}

QString AssetHelpers::BinaryAssetTypeToString(BinaryAssetType type)
{
	switch (type)
	{
		case Component:
            return QString("Component");
		case Texture:
            return QString("Texture");
		case ShapeOverride:
            return QString("ShapeOverride");
		case Animation:
            return QString("Animation");
		case ShapeOverridePost:
            return QString("ShapeOverridePost");
		default:
            throw QString("Asset: Invalid 'BinaryAssetType' value.\n");
	}
}
