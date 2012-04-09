//PHYSICS
#define	SPRING_STRENGTH         0.01f
#define BOUNCE                  0.2f

#define	GRAVITY					0.0f
#define FORCE_AMOUNT			10.0f

#define SECTOR_COUNT			1		// currently there is a bug at sector borders

#define CENTER_POS              ci::Vec3f::zero()


//Character Point Level
// 0 - Root
// 1 - Joint
// 2 - Feeler

#define CPL_ROOT         0
#define CPL_JOINT        1
#define CPL_FEELER       2


//Character Point Function
//
// Root:
// 0 - Middle
// 1 - Start
// 2 - End
//
// Feeler:
// 0 - Normal
// 1 - Stand

#define CPF_MIDDLE       0
#define CPF_START        1
#define CPF_END          2
#define CPF_SPACER       3

#define CPF_NORMAL       0
#define CPF_STAND        1

//MATH
#define TWOPI            6.28318531f