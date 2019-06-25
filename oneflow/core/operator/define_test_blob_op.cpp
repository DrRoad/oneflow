#include "oneflow/core/operator/define_test_blob_op.h"
#include "oneflow/core/job/sbp_signature_builder.h"

namespace oneflow {

void DefineTestBlobOp::InitFromOpConf() {
  CHECK(op_conf().has_define_test_blob_conf());
  if (op_conf().define_test_blob_conf().has_tick()) { EnrollInputBn("tick", false); }
  EnrollOutputBn("out", op_conf().define_test_blob_conf().has_diff());
}

const PbMessage& DefineTestBlobOp::GetCustomizedConf() const {
  return op_conf().define_test_blob_conf();
}

void DefineTestBlobOp::InferBlobDescs(
    std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
    const ParallelContext* parallel_ctx) const {
  BlobDesc* out_blob_desc = GetBlobDesc4BnInOp("out");
  const DefineTestBlobOpConf& conf = op_conf().define_test_blob_conf();
  Shape shape(conf.shape());
  out_blob_desc->mut_shape() = shape;
  out_blob_desc->set_data_type(conf.data_type());
  out_blob_desc->set_has_data_id_field(GlobalJobDesc().SizeOfOneDataId() > 0);
  out_blob_desc->set_has_col_num_field(false);
  out_blob_desc->set_has_dim0_valid_num_field(conf.has_dim0_valid_num());
  out_blob_desc->set_has_dim1_valid_num_field(conf.has_dim1_valid_num());
  out_blob_desc->set_has_dim2_valid_num_field(conf.has_dim2_valid_num());
  out_blob_desc->set_has_record_id_in_device_piece_field(!conf.record_id_in_device_piece().empty());
  out_blob_desc->set_max_col_num(1);
  if (conf.has_dim0_inner_shape()) {
    out_blob_desc->mut_dim0_inner_shape() = Shape(conf.dim0_inner_shape());
  }
  if (conf.has_dim0_valid_num()) { CHECK(conf.has_dim0_inner_shape()); }
}

void DefineTestBlobOp::InferHasBatchDim(
    std::function<bool*(const std::string&)> HasBatchDim4BnInOp) const {
  *HasBatchDim4BnInOp("out") = true;
}

void DefineTestBlobOp::GetSbpSignatures(SbpSignatureList* sbp_sig_list) const {
  SbpSignatureBuilder()
      .Split(input_bns(), 0)
      .Split(output_bns(), 0)
      .Build(sbp_sig_list->mutable_sbp_signature()->Add());
}

REGISTER_OP(OperatorConf::kDefineTestBlobConf, DefineTestBlobOp);

}  // namespace oneflow
