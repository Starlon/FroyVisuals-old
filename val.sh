#!/system/bin/sh
VGPARAMS='--error-limit=no'
export TMPDIR=/data/data/com.starlon.froyvisuals
exec /data/local/Inst/bin/valgrind $VGPARAMS $*


