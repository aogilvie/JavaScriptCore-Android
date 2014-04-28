/* CoreAnimation - CACFLayer.h

   Copyright (c) 2007 Apple Inc.
   All rights reserved. */

#ifndef CACFLAYER_H
#define CACFLAYER_H

#include <QuartzCore/CACFBase.h>
#include <QuartzCore/CATransform3D.h>

CA_EXTERN_C_BEGIN

/** Layer classes. **/

CA_EXTERN const CFStringRef kCACFLayer;
CA_EXTERN const CFStringRef kCACFTransformLayer;

/** Layer creation. **/

CA_EXTERN CACFLayerRef CACFLayerCreate (CFStringRef class_name);

CA_EXTERN CFStringRef CACFLayerGetClass (CACFLayerRef layer);

/** Layer hierarchy. **/

CA_EXTERN CACFLayerRef CACFLayerGetSuperlayer (CACFLayerRef layer);

CA_EXTERN CFArrayRef CACFLayerGetSublayers (CACFLayerRef layer);

CA_EXTERN void CACFLayerRemoveFromSuperlayer (CACFLayerRef layer);

CA_EXTERN void CACFLayerSetSublayers (CACFLayerRef layer, CFArrayRef
    sublayers);

CA_EXTERN void CACFLayerInsertSublayer (CACFLayerRef layer, CACFLayerRef
    sublayer, size_t idx);

CA_EXTERN CACFLayerRef CACFLayerGetMask (CACFLayerRef layer);

CA_EXTERN void CACFLayerSetMask (CACFLayerRef layer, CACFLayerRef mask);

/** Layout. **/

/* Associates a layout callback with 'layer'. */

CA_EXTERN void CACFLayerSetLayoutCallback (CACFLayerRef layer,
    void (*layoutSublayers) (CACFLayerRef layer));

/* Marks that layout should run on the layer needs before the next
 * update. The layer's layout callback will be called the next time the
 * associated context is flushed. */

CA_EXTERN void CACFLayerSetNeedsLayout (CACFLayerRef layer);

/* Invokes the layer's layout callback, this is done automatically on
 * commit if setNeedsLayout has been called. */

CA_EXTERN void CACFLayerLayoutSublayers (CACFLayerRef layer);

/** Drawing. **/

/* Associates a display callback with 'layer'. */

CA_EXTERN void CACFLayerSetDisplayCallback (CACFLayerRef layer,
    void (*display) (CACFLayerRef layer, CGContextRef ctx));

/* Marks that the layer needs to be redrawn before the next update. The
 * layer's display callback will be called the next time the associated
 * context is flushed. 'rect' may be null to denote the entire layer. */

CA_EXTERN void CACFLayerSetNeedsDisplay (CACFLayerRef layer,
    const CGRect *rect);

/* Invokes the layer's display callback if its bounds is non-empty,
 * this is done automatically on commit if setNeedsDisplay has been
 * called. */

CA_EXTERN void CACFLayerDisplay (CACFLayerRef layer);

/* Defines whether or not the backing store of the layer is opaque. */

CA_EXTERN void CACFLayerSetOpaque (CACFLayerRef layer, bool state);
CA_EXTERN bool CACFLayerIsOpaque (CACFLayerRef layer);

/** Layer animations. **/

CA_EXTERN void CACFLayerAddAnimation (CACFLayerRef layer, CFStringRef key,
    CACFAnimationRef anim);

CA_EXTERN void CACFLayerRemoveAnimation (CACFLayerRef layer, CFStringRef key);

CA_EXTERN void CACFLayerRemoveAllAnimations (CACFLayerRef layer);

/** Layer properties. **/

CA_EXTERN void *CACFLayerGetUserData (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetUserData (CACFLayerRef layer, void *data);

CA_EXTERN CFTimeInterval CACFLayerGetBeginTime (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetBeginTime (CACFLayerRef layer, CFTimeInterval t);

CA_EXTERN CFTimeInterval CACFLayerGetDuration (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetDuration (CACFLayerRef layer, CFTimeInterval dur);

CA_EXTERN float CACFLayerGetSpeed (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetSpeed (CACFLayerRef layer, float s);

CA_EXTERN CFTimeInterval CACFLayerGetTimeOffset (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetTimeOffset (CACFLayerRef layer, CFTimeInterval t);

CA_EXTERN float CACFLayerGetRepeatCount (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetRepeatCount (CACFLayerRef layer, float count);

CA_EXTERN CFTimeInterval CACFLayerGetRepeatDuration (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetRepeatDuration (CACFLayerRef layer,
    CFTimeInterval dur);

CA_EXTERN bool CACFLayerGetAutoreverses (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetAutoreverses (CACFLayerRef layer, bool state);

CA_EXTERN CFStringRef CACFLayerGetFillMode (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetFillMode (CACFLayerRef layer, CFStringRef str);

CA_EXTERN CGRect CACFLayerGetBounds (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetBounds (CACFLayerRef layer, CGRect r);

CA_EXTERN CGPoint CACFLayerGetPosition (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetPosition (CACFLayerRef layer, CGPoint p);

CA_EXTERN CGFloat CACFLayerGetZPosition (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetZPosition (CACFLayerRef layer, CGFloat z);

CA_EXTERN CGPoint CACFLayerGetAnchorPoint (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetAnchorPoint (CACFLayerRef layer, CGPoint p);

CA_EXTERN CGFloat CACFLayerGetAnchorPointZ (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetAnchorPointZ (CACFLayerRef layer, CGFloat z);

CA_EXTERN CATransform3D CACFLayerGetTransform (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetTransform (CACFLayerRef layer, CATransform3D t);

CA_EXTERN CATransform3D CACFLayerGetSublayerTransform (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetSublayerTransform (CACFLayerRef layer,
    CATransform3D t);

CA_EXTERN bool CACFLayerIsHidden (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetHidden (CACFLayerRef layer, bool state);

CA_EXTERN bool CACFLayerIsGeometryFlipped (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetGeometryFlipped (CACFLayerRef layer, bool state);

CA_EXTERN bool CACFLayerIsDoubleSided (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetDoubleSided (CACFLayerRef layer, bool state);

CA_EXTERN bool CACFLayerGetMasksToBounds (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetMasksToBounds (CACFLayerRef layer, bool state);

CA_EXTERN CFTypeRef CACFLayerGetContents (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetContents (CACFLayerRef layer, CFTypeRef obj);

CA_EXTERN CGRect CACFLayerGetContentsRect (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetContentsRect (CACFLayerRef layer, CGRect r);

CA_EXTERN CGRect CACFLayerGetContentsCenter (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetContentsCenter (CACFLayerRef layer, CGRect r);

CA_EXTERN CFStringRef CACFLayerGetContentsGravity (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetContentsGravity (CACFLayerRef layer,
    CFStringRef str);

CA_EXTERN CFStringRef CACFLayerGetMinificationFilter (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetMinificationFilter (CACFLayerRef layer,
    CFStringRef str);

CA_EXTERN float CACFLayerGetMinificationFilterBias (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetMinificationFilterBias (CACFLayerRef layer,
    float bias);

CA_EXTERN CFStringRef CACFLayerGetMagnificationFilter (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetMagnificationFilter (CACFLayerRef layer,
    CFStringRef str);

enum CACFEdgeAntialiasingMask
{
  kCACFLayerLeftEdge	= 1U << 0,
  kCACFLayerRightEdge	= 1U << 1,
  kCACFLayerBottomEdge	= 1U << 2,
  kCACFLayerTopEdge	= 1U << 3,
};

CA_EXTERN uint32_t CACFLayerGetEdgeAntialiasingMask (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetEdgeAntialiasingMask (CACFLayerRef layer,
    uint32_t mask);

CA_EXTERN CGColorRef CACFLayerGetBackgroundColor (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetBackgroundColor (CACFLayerRef layer, CGColorRef c);

CA_EXTERN CGFloat CACFLayerGetCornerRadius (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetCornerRadius (CACFLayerRef layer, CGFloat r);

CA_EXTERN CGFloat CACFLayerGetBorderWidth (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetBorderWidth (CACFLayerRef layer, CGFloat w);

CA_EXTERN CGColorRef CACFLayerGetBorderColor (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetBorderColor (CACFLayerRef layer, CGColorRef c);

CA_EXTERN float CACFLayerGetOpacity (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetOpacity (CACFLayerRef layer, float opacity);

CA_EXTERN CGColorRef CACFLayerGetShadowColor (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetShadowColor (CACFLayerRef layer, CGColorRef c);

CA_EXTERN float CACFLayerGetShadowOpacity (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetShadowOpacity (CACFLayerRef layer, float opacity);

CA_EXTERN CGSize CACFLayerGetShadowOffset (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetShadowOffset (CACFLayerRef layer, CGSize offset);

CA_EXTERN CGFloat CACFLayerGetShadowRadius (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetShadowRadius (CACFLayerRef layer, CGFloat r);

CA_EXTERN CFStringRef CACFLayerGetName (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetName (CACFLayerRef layer, CFStringRef str);

/** Convenience functions. **/

CA_EXTERN CGRect CACFLayerGetFrame (CACFLayerRef layer);
CA_EXTERN void CACFLayerSetFrame (CACFLayerRef layer, CGRect r);

CA_EXTERN CFTimeInterval CACFLayerConvertTime (CACFLayerRef from,
    CACFLayerRef to, CFTimeInterval t);

CA_EXTERN void CACFLayerConvertPoint (CACFLayerRef from, CACFLayerRef to,
    CGPoint *p);
CA_EXTERN void CACFLayerConvertRect (CACFLayerRef from, CACFLayerRef to,
    CGRect *r);
    
/** Layer type. **/

CA_EXTERN CFTypeID CACFLayerGetTypeID (void);

/** Layer `contentsGravity' values. **/

CA_EXTERN const CFStringRef kCACFGravityCenter;
CA_EXTERN const CFStringRef kCACFGravityTop;
CA_EXTERN const CFStringRef kCACFGravityBottom;
CA_EXTERN const CFStringRef kCACFGravityLeft;
CA_EXTERN const CFStringRef kCACFGravityRight;
CA_EXTERN const CFStringRef kCACFGravityTopLeft;
CA_EXTERN const CFStringRef kCACFGravityTopRight;
CA_EXTERN const CFStringRef kCACFGravityBottomLeft;
CA_EXTERN const CFStringRef kCACFGravityBottomRight;
CA_EXTERN const CFStringRef kCACFGravityResize;
CA_EXTERN const CFStringRef kCACFGravityResizeAspect;
CA_EXTERN const CFStringRef kCACFGravityResizeAspectFill;

/** Contents filter names. **/

CA_EXTERN const CFStringRef kCACFFilterLinear;
CA_EXTERN const CFStringRef kCACFFilterNearest;
CA_EXTERN const CFStringRef kCACFFilterTrilinear;

/** The animation key used for transitions. **/

CA_EXTERN const CFStringRef kCACFTransition;

CA_EXTERN_C_END

#endif /* CACFLAYER_H */
