#include "DRBDestroyProjectile.h"
#include "Projectile.h"

using namespace cute;

void DRBDestroyProjectile::on_destination_reached(Projectile &projectile) { projectile.deleteLater(); }
