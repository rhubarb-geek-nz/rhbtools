#!/bin/sh -e

ISSLACK=false

for d in $( . /etc/os-release ; echo $ID $ID_LIKE )
do
	case "$d" in
		slackware )
			ISSLACK=true
			;;
	esac
done

if $ISSLACK
then
	:
else
	exit 0
fi

INTDIR=$1
BASEDIR=$2
OUTDIR=$3

test -d "$INTDIR"
test -d "$BASEDIR"
test -d "$OUTDIR"

trap "chmod -R +w $INTDIR/slack.data ; rm -rf $INTDIR/slack.data" 0

mkdir -p "$INTDIR/slack.data/install" "$INTDIR/slack.data/root"

read PKGNAME
read VERSION
read PKGROOT

cat > "$INTDIR/slack.data/install/slack-desc"

ARCH=$(uname -m)
OSVER=$(. /etc/os-release ; echo $VERSION_ID)

case "$ARCH" in
	aarch64 | x86_64 )
		;;
	arm* )
		ARCH=arm
		;;
	* )
		ARCH=$(gcc -Q --help=target | grep "\-march=" | while read A B C; do echo $B; break; done)
		;;
esac

tar cf - -C "$BASEDIR" "$PKGROOT" | tar xf - -C "$INTDIR/slack.data"

if test -d "$INTDIR/slack.data/$PKGROOT"
then
	find  "$INTDIR/slack.data" -type d | xargs chmod +w

	(
		cd "$INTDIR/slack.data"
		find "$PKGROOT" -type l | while read N
		do
			D=$(dirname $N)
			B=$(basename $N)
			L=$(readlink $N)

			echo "( cd $D ; rm -rf $B )"
			echo "( cd $D ; ln -sf $L $B )"

			rm "$N"
		done
	) > "$INTDIR/slack.data/install/doinst.sh"
	
	if test 0 -eq $( wc -l < "$INTDIR/slack.data/install/doinst.sh" )
	then
		rm "$INTDIR/slack.data/install/doinst.sh"
	else
		chmod +x "$INTDIR/slack.data/install/doinst.sh"
	fi
fi

find  "$INTDIR/slack.data" -type d | xargs chmod -w

tar --create --gzip \
	 --owner=0 --group=0 \
	--file "$OUTDIR/$PKGNAME-$VERSION-$ARCH-1_slack$OSVER.tgz" \
	-C "$INTDIR/slack.data/root" \
		. \
	-C .. \
		$PKGROOT \
		$(cd "$INTDIR/slack.data" ; echo install/* )
