/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "nodo_comunicacion.h"

bool_t
xdr_DATOS (XDR *xdrs, DATOS *objp)
{
	register int32_t *buf;

	 if (!xdr_float (xdrs, &objp->vel_linear))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->vel_angular))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->angulo_rotacion))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->avance))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_RESULTADO (XDR *xdrs, RESULTADO *objp)
{
	register int32_t *buf;

	 if (!xdr_float (xdrs, &objp->pos_actual_x))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->pos_actual_y))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->orientacion_actual))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->error))
		 return FALSE;
	return TRUE;
}
