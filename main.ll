; ModuleID = 'main'
source_filename = "main"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define i64 @foo(i64 %n) {
entry:
  %i = alloca i64, align 8
  %n1 = alloca i64, align 8
  store i64 %n, ptr %n1, align 8
  store i64 0, ptr %i, align 8
  br label %loop_cond

loop_cond:                                        ; preds = %cond_post.loopexit, %entry
  %n2 = load i64, ptr %n1, align 8
  %eqtmp = icmp eq i64 %n2, 5
  br i1 %eqtmp, label %loop_post, label %until_body.preheader

until_body.preheader:                             ; preds = %loop_cond
  br label %until_body

until_body:                                       ; preds = %until_body, %until_body.preheader
  %n3 = load i64, ptr %n1, align 8
  %addtmp = add i64 %n3, 1
  store i64 %addtmp, ptr %n1, align 8
  %n4 = load i64, ptr %n1, align 8
  %eqtmp5 = icmp eq i64 %n4, 3
  br i1 %eqtmp5, label %until_body, label %cond_post.loopexit

cond_post.loopexit:                               ; preds = %until_body
  %i6 = load i64, ptr %i, align 8
  %addtmp7 = add i64 %i6, 1
  store i64 %addtmp7, ptr %i, align 8
  br label %loop_cond

loop_post:                                        ; preds = %loop_cond
  %i8 = load i64, ptr %i, align 8
  ret i64 %i8
}

define i64 @main() {
entry:
  %calltmp = call i64 @foo(i64 1)
  ret i64 %calltmp
}
