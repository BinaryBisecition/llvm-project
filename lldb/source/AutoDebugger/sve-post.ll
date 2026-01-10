; ModuleID = 'sve.ll'
source_filename = "sve.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: vscale_range(1,16)
define i32 @sve_test(ptr %call12) #0 {
bb:
  br label %bb48

bb48:                                             ; preds = %bb48, %bb
  %phi49 = phi i64 [ 0, %bb ], [ %add86, %bb48 ]
  %mul50 = mul i64 %phi49, 3
  %getelementptr53 = getelementptr i8, ptr %call12, i64 %mul50
  %0 = load <46 x i8>, ptr %getelementptr53, align 1
  %shufflevector = shufflevector <46 x i8> %0, <46 x i8> poison, <16 x i32> <i32 0, i32 3, i32 6, i32 9, i32 12, i32 15, i32 18, i32 21, i32 24, i32 27, i32 30, i32 33, i32 36, i32 39, i32 42, i32 45>
  %zext57 = zext <16 x i8> %shufflevector to <16 x i32>
  %mul58 = mul nuw nsw <16 x i32> %zext57, splat (i32 19595)
  %zext59 = zext <16 x i8> %shufflevector to <16 x i32>
  %mul60 = mul nuw nsw <16 x i32> %zext59, splat (i32 38470)
  %zext61 = zext <16 x i8> %shufflevector to <16 x i32>
  %mul62 = mul nuw nsw <16 x i32> %zext61, splat (i32 7471)
  %add63 = add nuw nsw <16 x i32> %mul58, splat (i32 32768)
  %add64 = add nuw nsw <16 x i32> %add63, %mul60
  %add65 = add nuw nsw <16 x i32> %add64, %mul62
  %lshr = lshr <16 x i32> %add65, splat (i32 16)
  %trunc66 = trunc nuw <16 x i32> %lshr to <16 x i8>
  %mul67 = mul nuw nsw <16 x i32> %zext57, splat (i32 32767)
  %mul68 = mul nuw <16 x i32> %zext59, splat (i32 16762097)
  %mul69 = mul nuw <16 x i32> %zext61, splat (i32 16759568)
  %add70 = add nuw nsw <16 x i32> %mul67, splat (i32 32768)
  %add71 = add nuw <16 x i32> %add70, %mul68
  %add72 = add <16 x i32> %add71, %mul69
  %lshr73 = lshr <16 x i32> %add72, splat (i32 16)
  %trunc74 = trunc <16 x i32> %lshr73 to <16 x i8>
  %mul75 = mul nuw nsw <16 x i32> %zext57, splat (i32 13282)
  %mul76 = mul nuw <16 x i32> %zext59, splat (i32 16744449)
  %mul77 = mul nuw nsw <16 x i32> %zext61, splat (i32 19485)
  %add78 = add nuw nsw <16 x i32> %mul75, splat (i32 32768)
  %add79 = add nuw <16 x i32> %add78, %mul76
  %add80 = add nuw <16 x i32> %add79, %mul77
  %lshr81 = lshr <16 x i32> %add80, splat (i32 16)
  %trunc82 = trunc <16 x i32> %lshr81 to <16 x i8>
  %shufflevector83 = shufflevector <16 x i8> %trunc66, <16 x i8> %trunc74, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %shufflevector84 = shufflevector <16 x i8> %trunc82, <16 x i8> poison, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  store <32 x i8> %shufflevector83, ptr %getelementptr53, align 1
  %add86 = add nuw i64 %phi49, 16
  %icmp87 = icmp eq i64 %add86, %mul50
  br i1 %icmp87, label %bb205, label %bb48

bb205:                                            ; preds = %bb48
  ret i32 0
}

attributes #0 = { vscale_range(1,16) "target-features"="+sve" }
